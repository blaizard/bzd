import warnings

warnings.simplefilter(action="ignore", category=FutureWarning)

import yfinance
import pathlib
import datetime
import typing
import traceback

from apps.trader_python.recording.recording_from_data import RecordingFromData, RecordingPairFromData
from apps.trader_python.recording.recording_to_path import recordingToPath
from apps.trader_python.recording.recording import Info, Event, EventKind
from bzd.utils.worker import Worker

Date = typing.Union[str, int]


class Generator:
	"""Generate recording using yahoo finance data."""

	def __init__(self, ticker: str) -> None:
		self.ticker = ticker

	@staticmethod
	def dateToDatetime(date: Date) -> datetime.datetime:
		if isinstance(date, str):
			return datetime.datetime.strptime(date, "%Y-%m-%d")
		else:
			return datetime.datetime.utcfromtimestamp(date)

	def generate(self,
	             startTime: typing.Optional[Date] = None,
	             endTime: typing.Optional[Date] = None) -> RecordingFromData:

		today = datetime.datetime.now()
		start = (today - datetime.timedelta(days=30)) if startTime is None else Generator.dateToDatetime(startTime)
		end = today if endTime is None else Generator.dateToDatetime(endTime)

		obj = yfinance.Ticker(self.ticker)
		info = obj.info
		currency = info["financialCurrency"]

		# Set info
		info = Info(
		    name=info.get("longName"),
		    timeZone=info.get("timeZoneShortName"),
		    countryHQ=info.get("country"),
		    industries=[info["industry"].lower()] if "industry" in info else [],
		    sectors=[info["sector"].lower()] if "sector" in info else [],
		    employees=info.get("fullTimeEmployees"),
		    resolution=60  # 1m
		)

		pairBid = RecordingPairFromData(currency, self.ticker, info)
		pairAsk = RecordingPairFromData(self.ticker, currency, info)

		slicesDays = 5
		while start < end:
			endHistory = (start +
			              datetime.timedelta(days=slicesDays)) if (start +
			                                                       datetime.timedelta(days=slicesDays)) <= end else end

			try:
				history = obj.history(start=start, end=endHistory, interval="1m")
			except IndexError as e:
				# There are some issues with "index 0 is out of bounds for axis 0 with size 0" due to some logging
				# this will ignore this.
				print(f"ERROR: {str(e)}")
				continue
			finally:
				start += datetime.timedelta(days=slicesDays)

			for index in history.index:
				timestamp = int(index.timestamp())
				open = history.loc[index, "Open"]
				high = history.loc[index, "High"]
				low = history.loc[index, "Low"]
				close = history.loc[index, "Close"]
				volume = history.loc[index, "Volume"]

				events = []
				if history.loc[index, "Dividends"]:
					events.append(Event(EventKind.DIVIDEND, history.loc[index, "Dividends"]))
				if history.loc[index, "Stock Splits"]:
					events.append(Event(EventKind.SPLIT, history.loc[index, "Stock Splits"]))

				pairBid.addOHLC(timestamp=timestamp,
				                open=open,
				                high=high,
				                low=low,
				                close=close,
				                volume=volume,
				                events=events)
				pairAsk.addOHLC(timestamp=timestamp,
				                open=open,
				                high=high,
				                low=low,
				                close=close,
				                volume=volume,
				                events=events)

		recording = RecordingFromData()
		recording.addPair(pairBid)
		recording.addPair(pairAsk)

		return recording


def workload(args: typing.Tuple[str, pathlib.Path], stdout: typing.TextIO) -> bool:

	ticker, path = args

	print(f"Processing '{ticker}'...")
	try:
		recording = Generator(ticker).generate()
		recordingToPath(recording, path)
	except Exception as e:
		stdout.write(f"{ticker}:\n")
		stdout.write(str(e) + "\n")
		stdout.write(str(traceback.format_exc()))
		return False

	return True


if __name__ == "__main__":

	tickerList = [
	    "MMM", "AOS", "ABT", "ABBV", "ACN", "ATVI", "ADM", "ADBE", "ADP", "AES", "AFL", "A", "ABNB", "APD", "AKAM",
	    "ALK", "ALB", "ARE", "ALGN", "ALLE", "LNT", "ALL", "GOOGL", "GOOG", "MO", "AMZN", "AMCR", "AMD", "AEE", "AAL",
	    "AEP", "AXP", "AIG", "AMT", "AWK", "AMP", "AME", "AMGN", "APH", "ADI", "ANSS", "AON", "APA", "AAPL", "AMAT",
	    "APTV", "ACGL", "ANET", "AJG", "AIZ", "T", "ATO", "ADSK", "AZO", "AVB", "AVY", "AXON", "BKR", "BALL", "BAC",
	    "BBWI", "BAX", "BDX", "WRB", "BBY", "BIO", "TECH", "BIIB", "BLK", "BX", "BK", "BA", "BKNG", "BWA", "BXP", "BSX",
	    "BMY", "AVGO", "BR", "BRO", "BG", "CHRW", "CDNS", "CZR", "CPT", "CPB", "COF", "CAH", "KMX", "CCL", "CARR",
	    "CTLT", "CAT", "CBOE", "CBRE", "CDW", "CE", "COR", "CNC", "CNP", "CDAY", "CF", "CRL", "SCHW", "CHTR", "CVX",
	    "CMG", "CB", "CHD", "CI", "CINF", "CTAS", "CSCO", "C", "CFG", "CLX", "CME", "CMS", "KO", "CTSH", "CL", "CMCSA",
	    "CMA", "CAG", "COP", "ED", "STZ", "CEG", "COO", "CPRT", "GLW", "CTVA", "CSGP", "COST", "CTRA", "CCI", "CSX",
	    "CMI", "CVS", "DHI", "DHR", "DRI", "DVA", "DE", "DAL", "XRAY", "DVN", "DXCM", "FANG", "DLR", "DFS", "DIS", "DG",
	    "DLTR", "D", "DPZ", "DOV", "DOW", "DTE", "DUK", "DD", "DXC", "EMN", "ETN", "EBAY", "ECL", "EIX", "EW", "EA",
	    "ELV", "LLY", "EMR", "ENPH", "ETR", "EOG", "EPAM", "EQT", "EFX", "EQIX", "EQR", "ESS", "EL", "ETSY", "EG",
	    "EVRG", "ES", "EXC", "EXPE", "EXPD", "EXR", "XOM", "FFIV", "FDS", "FICO", "FAST", "FRT", "FDX", "FITB", "FSLR",
	    "FE", "FIS", "FI", "FLT", "FMC", "F", "FTNT", "FTV", "FOXA", "FOX", "BEN", "FCX", "GRMN", "IT", "GEHC", "GEN",
	    "GNRC", "GD", "GE", "GIS", "GM", "GPC", "GILD", "GL", "GPN", "GS", "HAL", "HIG", "HAS", "HCA", "PEAK", "HSIC",
	    "HSY", "HES", "HPE", "HLT", "HOLX", "HD", "HON", "HRL", "HST", "HWM", "HPQ", "HUM", "HBAN", "HII", "IBM", "IEX",
	    "IDXX", "ITW", "ILMN", "INCY", "IR", "PODD", "INTC", "ICE", "IFF", "IP", "IPG", "INTU", "ISRG", "IVZ", "INVH",
	    "IQV", "IRM", "JBHT", "JKHY", "J", "JNJ", "JCI", "JPM", "JNPR", "K", "KVUE", "KDP", "KEY", "KEYS", "KMB", "KIM",
	    "KMI", "KLAC", "KHC", "KR", "LHX", "LH", "LRCX", "LW", "LVS", "LDOS", "LEN", "LIN", "LYV", "LKQ", "LMT", "L",
	    "LOW", "LYB", "MTB", "MRO", "MPC", "MKTX", "MAR", "MMC", "MLM", "MAS", "MA", "MTCH", "MKC", "MCD", "MCK", "MDT",
	    "MRK", "META", "MET", "MTD", "MGM", "MCHP", "MU", "MSFT", "MAA", "MRNA", "MHK", "MOH", "TAP", "MDLZ", "MPWR",
	    "MNST", "MCO", "MS", "MOS", "MSI", "MSCI", "NDAQ", "NTAP", "NFLX", "NEM", "NWSA", "NWS", "NEE", "NKE", "NI",
	    "NDSN", "NSC", "NTRS", "NOC", "NCLH", "NRG", "NUE", "NVDA", "NVR", "NXPI", "ORLY", "OXY", "ODFL", "OMC", "ON",
	    "OKE", "ORCL", "OGN", "OTIS", "PCAR", "PKG", "PANW", "PARA", "PH", "PAYX", "PAYC", "PYPL", "PNR", "PEP", "PFE",
	    "PCG", "PM", "PSX", "PNW", "PXD", "PNC", "POOL", "PPG", "PPL", "PFG", "PG", "PGR", "PLD", "PRU", "PEG", "PTC",
	    "PSA", "PHM", "QRVO", "PWR", "QCOM", "DGX", "RL", "RJF", "RTX", "O", "REG", "REGN", "RF", "RSG", "RMD", "RVTY",
	    "RHI", "ROK", "ROL", "ROP", "ROST", "RCL", "SPGI", "CRM", "SBAC", "SLB", "STX", "SEE", "SRE", "NOW", "SHW",
	    "SPG", "SWKS", "SJM", "SNA", "SEDG", "SO", "LUV", "SWK", "SBUX", "STT", "STLD", "STE", "SYK", "SYF", "SNPS",
	    "SYY", "TMUS", "TROW", "TTWO", "TPR", "TRGP", "TGT", "TEL", "TDY", "TFX", "TER", "TSLA", "TXN", "TXT", "TMO",
	    "TJX", "TSCO", "TT", "TDG", "TRV", "TRMB", "TFC", "TYL", "TSN", "USB", "UDR", "ULTA", "UNP", "UAL", "UPS",
	    "URI", "UNH", "UHS", "VLO", "VTR", "VRSN", "VRSK", "VZ", "VRTX", "VFC", "VTRS", "VICI", "V", "VMC", "WAB",
	    "WBA", "WMT", "WBD", "WM", "WAT", "WEC", "WFC", "WELL", "WST", "WDC", "WRK", "WY", "WHR", "WMB", "WTW", "GWW",
	    "WYNN", "XEL", "XYL", "YUM", "ZBRA", "ZBH", "ZION", "ZTS"
	]

	worker = Worker(workload)

	with worker.start() as w:
		for index in range(len(tickerList)):
			ticker = tickerList[index]
			w.add(
			    [ticker, pathlib.Path("~/Documents/recordings/yfinance")],
			    timeoutS=60,
			)
		for result in w.data():
			if (not bool(result.getResult())) or result.isException():
				print(result.getOutput().strip())
