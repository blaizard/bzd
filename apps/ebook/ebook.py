import argparse
import pathlib
import tempfile
import typing
import enum
import shutil
import math
import multiprocessing

from apps.ebook.calibre.remove_drm import RemoveDRM
from apps.ebook.epub.epub import EPub
from apps.ebook.comics.cbz import Cbz
from apps.ebook.comics.cbr import Cbr
from apps.ebook.pdf.pdf_to_images import PdfToImages
from apps.ebook.pillow.images_to_pdf import ImagesToPdf
from apps.ebook.pillow.images_to_cover import ImagesToCover
from apps.ebook.pillow.images_converter import ImagesConverter
from apps.ebook.flow import ActionInterface, FlowRegistry, FlowEnum, FlowSchemaType, FlowContext
from apps.ebook.providers import ProviderEbook, ProviderEbookMetadata, ProviderPdf, providerSerialize, providerDeserialize
from apps.ebook.utils import sizeToString, percentToString


class FlowSchemaPdf(FlowEnum):
	epub: FlowSchemaType = ["removeDRM", "epub", "imagesConverter", "imagesToPdf"]
	cbz: FlowSchemaType = ["cbz", "imagesConverter", "imagesToPdf"]
	cbr: FlowSchemaType = ["cbr", "imagesConverter", "imagesToPdf"]
	pdf: FlowSchemaType = ["pdfToImages", "imagesConverter", "imagesToPdf"]
	auto: FlowSchemaType = ["discover"]


class FlowSchemaCover(FlowEnum):
	pdf: FlowSchemaType = ["pdfToCoverImage", "imagesToCover"]
	auto: FlowSchemaType = ["discover"]


flowSchemas = {
    "pdf": FlowSchemaPdf,
    "cover": FlowSchemaCover,
}


class Discover(ActionInterface):
	"""Discover the ebook format."""

	ProviderInput = ProviderEbook
	ProviderOutput = ProviderEbook

	def __init__(self, ebookFormat: typing.Optional[str]) -> None:
		self.ebookFormat = ebookFormat

	def process(self, provider: ProviderEbook, directory: pathlib.Path,
	            context: FlowContext) -> typing.List[typing.Tuple[ProviderEbook, typing.Optional[FlowEnum]]]:

		ebookFormat = self.ebookFormat or provider.ebook.suffix.removeprefix(".").lower()
		print(f"Ebook of format '{ebookFormat}'.")
		if hasattr(context.schema, ebookFormat):
			return [(provider, getattr(context.schema, ebookFormat))]

		assert False, f"Unsupported ebook extension '.{ebookFormat}', only the following are supported for this flow: {str(', '.join([value.name for value in context.schema]))}."


class SandboxDirectory:

	def __init__(self, path: pathlib.Path) -> None:
		self.path = path
		self.path.mkdir(parents=True, exist_ok=True)

	@property
	def name(self) -> str:
		return str(self.path)

	def cleanup(self) -> None:
		pass  # ignore cleanup


def createSiblingOutput(reference: pathlib.Path) -> pathlib.Path:
	"""Generate the name of a siblings file."""

	def generateExtra() -> typing.Generator[str, None, None]:
		yield ""
		counter = 1
		while True:
			yield f".{counter:03}"
			counter += 1

	for extra in generateExtra():
		path = reference.parent / f"{reference.stem}{extra}{reference.suffix}"
		if path.exists():
			continue
		return path
	assert False, "Unreachable"


def runFlow(provider: ProviderEbook, flowRegistry: FlowRegistry, directory: pathlib.Path) -> None:
	"""Run the given flow."""

	flow = flowRegistry.restoreOrReset(directory, ["discover"], provider)
	providers = flow.run(uid=provider.hash())

	print("--- result")
	for output in providers:
		if hasattr(output, "outputs"):
			for path in output.outputs:
				destination = createSiblingOutput(provider.ebook.parent / path.name)
				# Note, we use this instead of shutil.copy/move to avoid copying metadata.
				# This is important for some filesystem that do not support copying metadata.
				shutil.copyfile(path, destination)
				sizeStr = sizeToString(destination.stat().st_size)
				print(f"Output {destination} ({sizeStr}).")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="e-book to pdf.")
	parser.add_argument("--key",
	                    dest="keys",
	                    action="append",
	                    type=pathlib.Path,
	                    default=[],
	                    help="Keys to be added to calibre.")
	parser.add_argument("--calibre-config",
	                    type=pathlib.Path,
	                    default=pathlib.Path.home() / ".config/calibre",
	                    help="Calibre configuration path.")
	parser.add_argument("--format", type=str, default=None, help="Assume a specific format for the ebook.")
	parser.add_argument(
	    "--coefficient",
	    type=float,
	    default=1.0,
	    help="Coefficient to be used to find outliers, the lower the value, the more outliers will be removed.")
	parser.add_argument("--zoom", type=float, default=1.0, help="Zoom coefficient to be used.")
	parser.add_argument("--clean", action="store_true", help="Start from a clean sandbox if any.")
	parser.add_argument("--max-dpi", type=int, default=None, help="Maximum DPI for an image.")
	parser.add_argument("--scale",
	                    type=float,
	                    default=None,
	                    help="Reduce images by the given scale (this number must be <= 1).")

	parser.add_argument(
	    "--sandbox",
	    type=pathlib.Path,
	    help=
	    "Directory to use to store the intermediate data, this directory will be kept when the program terminated. Also if interrupted, the process will restart from the last step."
	)
	parser.add_argument("action", type=str, choices=flowSchemas.keys(), help="The type of action to perform.")
	parser.add_argument("ebook", type=pathlib.Path, help="The ebook file or directory to sanitize.")

	args = parser.parse_args()

	actions = {
	    "removeDRM": RemoveDRM(calibreConfigPath=args.calibre_config),
	    "epub": EPub(zoom=args.zoom),
	    "cbz": Cbz(coefficient=args.coefficient),
	    "cbr": Cbr(coefficient=args.coefficient),
	    "imagesConverter": ImagesConverter(maxDPI=args.max_dpi, scale=args.scale),
	    "imagesToPdf": ImagesToPdf(),
	    "imagesToCover": ImagesToCover(),
	    "pdfToImages": PdfToImages(maxDPI=args.max_dpi),
	    "pdfToCoverImage": PdfToImages(maxDPI=100, maxImages=1),
	    "discover": Discover(ebookFormat=args.format)
	}

	schema = flowSchemas[args.action]

	if args.clean:
		if args.sandbox is not None and args.sandbox.exists():
			print(f"Cleaning sandbox {args.sandbox}.")
			shutil.rmtree(args.sandbox)

	assert args.ebook.exists(), f"File or directory '{args.ebook}' does not exists."
	if args.ebook.is_file():
		ebooks = [args.ebook]
	else:
		allFiles = args.ebook.rglob("**/*")
		ebooks = [f for f in allFiles if hasattr(schema, f.suffix.removeprefix(".").lower())]

	for index, ebook in enumerate(ebooks):
		directory = tempfile.TemporaryDirectory() if args.sandbox is None else SandboxDirectory(path=args.sandbox)
		try:
			print(f"=== [{index + 1}/{len(ebooks)}] Running flow '{args.action}' on {ebook} ===")
			provider = ProviderEbook(ebook=ebook, keys=args.keys, metadata=ProviderEbookMetadata(title=ebook.stem))

			# Run the flow in isolation, this prevents any memory leaks/hogs which becomes problematic with batch runs.
			p = multiprocessing.Process(target=runFlow,
			                            args=(provider, FlowRegistry(schema=flowSchemas[args.action],
			                                                         actions=actions), pathlib.Path(directory.name)))
			p.start()
			p.join()

		finally:
			directory.cleanup()  # type: ignore
