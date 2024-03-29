import logging

# Default logger
logging.basicConfig(
    level=logging.INFO,
    format="[%(asctime)s] [%(levelname)s] [%(filename)s:%(lineno)d] %(message)s",
)


class Logger:
	info = logging.info
	warning = logging.warning
	error = logging.error
