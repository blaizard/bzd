import pathlib

import pymupdf
from apps.ebook.flow import ActionInterface, FlowContext, FlowEnum
from apps.ebook.providers import ProviderEbook, ProviderImages
from apps.ebook.utils import estimatePageDPIs, percentToString, sizeToString


class PdfToImages(ActionInterface):
	"""Extract images from a pdf."""

	ProviderInput = ProviderEbook
	ProviderOutput = ProviderImages

	def __init__(
		self,
		maxDPI: int | None = None,
		maxImages: int | None = None,
	) -> None:
		super().__init__()
		self.maxDPI = maxDPI
		self.maxImages = maxImages

	def process(
		self, provider: ProviderEbook, directory: pathlib.Path, context: FlowContext
	) -> list[tuple[ProviderImages, FlowEnum | None]]:

		allImages = []
		with pymupdf.open(provider.ebook) as doc:  # type: ignore
			# Estimate the DPI of the pages, as the dpi attribute from get_pixmap is not reliable.
			dimensions: list[tuple[int, int]] = []
			for page in doc:
				rect = page.bound()
				dimensions.append(
					(
						rect.width,
						rect.height,
					)
				)
			maybeEstimatedDPIs = estimatePageDPIs(dimensions=dimensions)

			pageTotal = len(doc)
			for pageIndex, page in enumerate(doc):
				if self.maxImages is not None and pageIndex >= self.maxImages:
					print(f"Reached maximum number of images ({self.maxImages}), stopping extraction.")
					break

				messages = []
				pageArgs = {}
				if self.maxDPI is not None:
					if maybeEstimatedDPIs is not None:
						zoom = self.maxDPI / maybeEstimatedDPIs[pageIndex]
						pageArgs["matrix"] = pymupdf.Matrix(zoom, zoom)  # type: ignore
						messages.append(f"zoom x{zoom:.1f} to match target DPI ({self.maxDPI})")
					else:
						pageArgs["dpi"] = self.maxDPI  # type: ignore
						messages.append(f"matching target DPI ({self.maxDPI})")

				pix = page.get_pixmap(**pageArgs)
				output = directory / f"{pageIndex}.jpg"
				pix.save(output)
				allImages.append(output)
				sizeStr = sizeToString(output.stat().st_size)
				print(
					f"Extracted image '{output.name}' ({sizeStr}) ({percentToString(pageIndex / pageTotal)}): {'; '.join(messages)}"
				)

		return [(ProviderImages(images=allImages, metadata=provider.metadata), None)]
