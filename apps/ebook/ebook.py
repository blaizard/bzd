import argparse
import pathlib
import tempfile
import typing
import enum
import shutil

from apps.ebook.calibre.remove_drm import RemoveDRM
from apps.ebook.epub.epub import EPub
from apps.ebook.comics.cbz import Cbz
from apps.ebook.comics.cbr import Cbr
from apps.ebook.pillow.images_to_pdf import ImagesToPdf
from apps.ebook.flow import ActionInterface, FlowRegistry, FlowEnum, FlowSchemaType
from apps.ebook.providers import ProviderEbook, ProviderEbookMetadata, ProviderPdf, providerSerialize, providerDeserialize


class FlowSchema(FlowEnum):
	epub: FlowSchemaType = ["removeDRM", "epub", "imagesToPdf"]
	cbz: FlowSchemaType = ["cbz", "imagesToPdf"]
	cbr: FlowSchemaType = ["cbr", "imagesToPdf"]
	auto: FlowSchemaType = ["discover"]


class Discover(ActionInterface):
	"""Discover the ebook format."""

	ProviderInput = ProviderEbook
	ProviderOutput = ProviderEbook

	def __init__(self, ebookFormat: typing.Optional[str]) -> None:
		self.ebookFormat = ebookFormat

	def process(self, provider: ProviderEbook,
	            directory: pathlib.Path) -> typing.List[typing.Tuple[ProviderEbook, typing.Optional[FlowEnum]]]:

		ebookFormat = self.ebookFormat or provider.ebook.suffix.removeprefix(".").lower()
		print(f"Ebook of format '{ebookFormat}'.")
		if hasattr(FlowSchema, ebookFormat):
			return [(provider, getattr(FlowSchema, ebookFormat))]

		assert False, f"Unsupported ebook format: {ebookFormat}, only the following are supported: {str(', '.join([value.name for value in FlowSchema]))}."


class SandboxDirectory:

	def __init__(self, path: pathlib.Path) -> None:
		self.path = path
		self.path.mkdir(parents=True, exist_ok=True)

	@property
	def name(self) -> str:
		return str(self.path)

	def cleanup(self) -> None:
		pass  # ignore cleanup


def createSiblingOutput(reference: pathlib.Path, extension: str) -> pathlib.Path:
	"""Generate the name of a siblings file."""

	def generateExtra() -> typing.Generator[str, None, None]:
		yield ""
		counter = 1
		while True:
			yield f".{counter:03}"
			counter += 1

	for extra in generateExtra():
		path = reference.parent / f"{reference.stem}{extra}.{extension}"
		if path.exists():
			continue
		return path
	assert False, "Unreachable"


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
	parser.add_argument("--coefficient", type=float, default=1.0, help="Coefficient to be used to find outliers.")
	parser.add_argument("--zoom", type=float, default=1.0, help="Zoom coefficient to be used.")
	parser.add_argument(
	    "--sandbox",
	    type=pathlib.Path,
	    help=
	    "Directory to use to store the intermediate data, this directory will be kept when the program terminated. Also if interrupted, the process will restart from the last step."
	)
	parser.add_argument("ebook", type=pathlib.Path, help="The ebook file to sanitize.")

	args = parser.parse_args()

	actions = {
	    "removeDRM": RemoveDRM(calibreConfigPath=args.calibre_config),
	    "epub": EPub(zoom=args.zoom),
	    "cbz": Cbz(coefficient=args.coefficient),
	    "cbr": Cbr(coefficient=args.coefficient),
	    "imagesToPdf": ImagesToPdf(),
	    "discover": Discover(ebookFormat=args.format)
	}

	flows = FlowRegistry(schema=FlowSchema, actions=actions)
	directory = tempfile.TemporaryDirectory() if args.sandbox is None else SandboxDirectory(path=args.sandbox)
	try:
		provider = ProviderEbook(ebook=args.ebook,
		                         keys=args.keys,
		                         metadata=ProviderEbookMetadata(title=args.ebook.stem))
		flow = flows.restoreOrReset(pathlib.Path(directory.name), FlowSchema.auto, provider)  # type: ignore
		providers = flow.run()

		print("--- result")
		for output in providers:
			if isinstance(output, ProviderPdf):
				path = createSiblingOutput(args.ebook.parent / output.path.name, "pdf")
				shutil.move(output.path, path)
				print(f"Output {path}.")

	finally:
		directory.cleanup()  # type: ignore
