import logging

# Default logger
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] [%(levelname)s] [%(name)s] [%(filename)s:%(lineno)d] %(message)s",
)


class Logger:

	def __init__(self, name: str) -> None:
		self.logger = logging.getLogger(name)

	@property
	def info(self):
		return self.logger.info

	@property
	def warning(self):
		return self.logger.warning

	@property
	def error(self):
		return self.logger.error
