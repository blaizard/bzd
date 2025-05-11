import argparse
import pathlib
import tempfile
import typing
import enum
import shutil

from tools.docker_images.ebook.calibre.remove_drm import RemoveDRM
from tools.docker_images.ebook.epub.epub import EPub
from tools.docker_images.ebook.pillow.images_to_pdf import ImagesToPdf
from tools.docker_images.ebook.flow import ActionInterface, FlowRegistry, FlowEnum, FlowSchemaType
from tools.docker_images.ebook.providers import ProviderEbook, providerSerialize, providerDeserialize


class FlowSchema(FlowEnum):
	epub: FlowSchemaType = ["removeDRM", "epub", "imagesToPdf"]
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
		if ebookFormat == "epub":
			return [(provider, FlowSchema.epub)]

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
	    "--sandbox",
	    type=pathlib.Path,
	    help=
	    "Directory to use to store the intermediate data, this directory will be kept when the program terminated. Also if interrupted, the process will restart from the last step."
	)
	parser.add_argument("ebook", type=pathlib.Path, help="The ebook file to sanitize.")

	args = parser.parse_args()

	actions = {
	    "removeDRM": RemoveDRM(calibreConfigPath=args.calibre_config),
	    "epub": EPub(),
	    "imagesToPdf": ImagesToPdf(),
	    "discover": Discover(ebookFormat=args.format)
	}

	flows = FlowRegistry(schema=FlowSchema, actions=actions)
	directory = tempfile.TemporaryDirectory() if args.sandbox is None else SandboxDirectory(path=args.sandbox)
	try:
		provider = ProviderEbook(ebook=args.ebook, keys=args.keys)
		flow = flows.restoreOrReset(pathlib.Path(directory.name), FlowSchema.auto, provider)  # type: ignore
		flow.run()

	finally:
		directory.cleanup()  # type: ignore

	# bazel run tools/docker_images/ebook -- --key $(pwd)/temp/Adobe_PrivateLicenseKey--anonymous.der $(pwd)/temp/temp.epub
	# tools/docker_images/ebook/ebook.py --key $(pwd)/temp/Adobe_PrivateLicenseKey--anonymous.der --format pdf $(pwd)/temp/temp.epub
	# bazel run tools/docker_images/ebook -- $(pwd)/temp/temp_sanitized.nodrm.epub
