import ExceptionFactory from "../core/exception.mjs";

const Exception = ExceptionFactory("currency");

class Currency {
	constructor() {
		this.data = Object.freeze({
			usd: { symbol: "$", name: "US Dollar", format: "$ {}" },
			cad: { symbol: "$", name: "Canadian Dollar", format: "$ {}" },
			eur: { symbol: "€", name: "Euro", format: "{} €" },
			aed: { symbol: "د.إ.‏", name: "United Arab Emirates Dirham", format: "{} د.إ.‏" },
			afn: { symbol: "؋", name: "Afghan Afghani", format: "{} ؋" },
			all: { symbol: "Lek", name: "Albanian Lek", format: "{} Lek" },
			amd: { symbol: "դր.", name: "Armenian Dram", format: "{} դր." },
			ars: { symbol: "$", name: "Argentine Peso", format: "{} $" },
			aud: { symbol: "$", name: "Australian Dollar", format: "{} $" },
			azn: { symbol: "ман.", name: "Azerbaijani Manat", format: "{} ман." },
			bam: { symbol: "KM", name: "Bosnia-Herzegovina Convertible Mark", format: "{} KM" },
			bdt: { symbol: "৳", name: "Bangladeshi Taka", format: "{} ৳" },
			bgn: { symbol: "лв.", name: "Bulgarian Lev", format: "{} лв." },
			bhd: { symbol: "د.ب.‏", name: "Bahraini Dinar", format: "{} د.ب.‏" },
			bif: { symbol: "FBu", name: "Burundian Franc", format: "{} FBu" },
			bnd: { symbol: "$", name: "Brunei Dollar", format: "{} $" },
			bob: { symbol: "Bs", name: "Bolivian Boliviano", format: "{} Bs" },
			brl: { symbol: "R$", name: "Brazilian Real", format: "{} R$" },
			bwp: { symbol: "P", name: "Botswanan Pula", format: "{} P" },
			byr: { symbol: "BYR", name: "Belarusian Ruble", format: "{} BYR" },
			bzd: { symbol: "$", name: "Belize Dollar", format: "{} $" },
			cdf: { symbol: "FrCD", name: "Congolese Franc", format: "{} FrCD" },
			chf: { symbol: "CHF", name: "Swiss Franc", format: "{} CHF" },
			clp: { symbol: "$", name: "Chilean Peso", format: "{} $" },
			cny: { symbol: "CN¥", name: "Chinese Yuan", format: "{} CN¥" },
			cop: { symbol: "$", name: "Colombian Peso", format: "{} $" },
			crc: { symbol: "₡", name: "Costa Rican Colón", format: "{} ₡" },
			cve: { symbol: "CV$", name: "Cape Verdean Escudo", format: "{} CV$" },
			czk: { symbol: "Kč", name: "Czech Republic Koruna", format: "{} Kč" },
			djf: { symbol: "Fdj", name: "Djiboutian Franc", format: "{} Fdj" },
			dkk: { symbol: "kr", name: "Danish Krone", format: "{} kr" },
			dop: { symbol: "RD$", name: "Dominican Peso", format: "{} RD$" },
			dzd: { symbol: "د.ج.‏", name: "Algerian Dinar", format: "{} د.ج.‏" },
			eek: { symbol: "kr", name: "Estonian Kroon", format: "{} kr" },
			egp: { symbol: "ج.م.‏", name: "Egyptian Pound", format: "{} ج.م.‏" },
			ern: { symbol: "Nfk", name: "Eritrean Nakfa", format: "{} Nfk" },
			etb: { symbol: "Br", name: "Ethiopian Birr", format: "{} Br" },
			gbp: { symbol: "£", name: "British Pound Sterling", format: "{} £" },
			gel: { symbol: "GEL", name: "Georgian Lari", format: "{} GEL" },
			ghs: { symbol: "GH₵", name: "Ghanaian Cedi", format: "{} GH₵" },
			gnf: { symbol: "FG", name: "Guinean Franc", format: "{} FG" },
			gtq: { symbol: "Q", name: "Guatemalan Quetzal", format: "{} Q" },
			hkd: { symbol: "$", name: "Hong Kong Dollar", format: "{} $" },
			hnl: { symbol: "L", name: "Honduran Lempira", format: "{} L" },
			hrk: { symbol: "kn", name: "Croatian Kuna", format: "{} kn" },
			huf: { symbol: "Ft", name: "Hungarian Forint", format: "{} Ft" },
			idr: { symbol: "Rp", name: "Indonesian Rupiah", format: "{} Rp" },
			ils: { symbol: "₪", name: "Israeli New Sheqel", format: "{} ₪" },
			inr: { symbol: "টকা", name: "Indian Rupee", format: "{} টকা" },
			iqd: { symbol: "د.ع.‏", name: "Iraqi Dinar", format: "{} د.ع.‏" },
			irr: { symbol: "﷼", name: "Iranian Rial", format: "{} ﷼" },
			isk: { symbol: "kr", name: "Icelandic Króna", format: "{} kr" },
			jmd: { symbol: "$", name: "Jamaican Dollar", format: "{} $" },
			jod: { symbol: "د.أ.‏", name: "Jordanian Dinar", format: "{} د.أ.‏" },
			jpy: { symbol: "￥", name: "Japanese Yen", format: "{} ￥" },
			kes: { symbol: "Ksh", name: "Kenyan Shilling", format: "{} Ksh" },
			khr: { symbol: "៛", name: "Cambodian Riel", format: "{} ៛" },
			kmf: { symbol: "FC", name: "Comorian Franc", format: "{} FC" },
			krw: { symbol: "₩", name: "South Korean Won", format: "{} ₩" },
			kwd: { symbol: "د.ك.‏", name: "Kuwaiti Dinar", format: "{} د.ك.‏" },
			kzt: { symbol: "тңг.", name: "Kazakhstani Tenge", format: "{} тңг." },
			lbp: { symbol: "ل.ل.‏", name: "Lebanese Pound", format: "{} ل.ل.‏" },
			lkr: { symbol: "SL Re", name: "Sri Lankan Rupee", format: "{} SL Re" },
			ltl: { symbol: "Lt", name: "Lithuanian Litas", format: "{} Lt" },
			lvl: { symbol: "Ls", name: "Latvian Lats", format: "{} Ls" },
			lyd: { symbol: "د.ل.‏", name: "Libyan Dinar", format: "{} د.ل.‏" },
			mad: { symbol: "د.م.‏", name: "Moroccan Dirham", format: "{} د.م.‏" },
			mdl: { symbol: "MDL", name: "Moldovan Leu", format: "{} MDL" },
			mga: { symbol: "MGA", name: "Malagasy Ariary", format: "{} MGA" },
			mkd: { symbol: "MKD", name: "Macedonian Denar", format: "{} MKD" },
			mmk: { symbol: "K", name: "Myanma Kyat", format: "{} K" },
			mop: { symbol: "MOP$", name: "Macanese Pataca", format: "{} MOP$" },
			mur: { symbol: "MURs", name: "Mauritian Rupee", format: "{} MURs" },
			mxn: { symbol: "$", name: "Mexican Peso", format: "{} $" },
			myr: { symbol: "RM", name: "Malaysian Ringgit", format: "{} RM" },
			mzn: { symbol: "MTn", name: "Mozambican Metical", format: "{} MTn" },
			nad: { symbol: "N$", name: "Namibian Dollar", format: "{} N$" },
			ngn: { symbol: "₦", name: "Nigerian Naira", format: "{} ₦" },
			nio: { symbol: "C$", name: "Nicaraguan Córdoba", format: "{} C$" },
			nok: { symbol: "kr", name: "Norwegian Krone", format: "{} kr" },
			npr: { symbol: "नेरू", name: "Nepalese Rupee", format: "{} नेरू" },
			nzd: { symbol: "$", name: "New Zealand Dollar", format: "{} $" },
			omr: { symbol: "ر.ع.‏", name: "Omani Rial", format: "{} ر.ع.‏" },
			pab: { symbol: "B/.", name: "Panamanian Balboa", format: "{} B/." },
			pen: { symbol: "S/.", name: "Peruvian Nuevo Sol", format: "{} S/." },
			php: { symbol: "₱", name: "Philippine Peso", format: "{} ₱" },
			pkr: { symbol: "₨", name: "Pakistani Rupee", format: "{} ₨" },
			pln: { symbol: "zł", name: "Polish Zloty", format: "{} zł" },
			pyg: { symbol: "₲", name: "Paraguayan Guarani", format: "{} ₲" },
			qar: { symbol: "ر.ق.‏", name: "Qatari Rial", format: "{} ر.ق.‏" },
			ron: { symbol: "RON", name: "Romanian Leu", format: "{} RON" },
			rsd: { symbol: "дин.", name: "Serbian Dinar", format: "{} дин." },
			rub: { symbol: "руб.", name: "Russian Ruble", format: "{} руб." },
			rwf: { symbol: "FR", name: "Rwandan Franc", format: "{} FR" },
			sar: { symbol: "ر.س.‏", name: "Saudi Riyal", format: "{} ر.س.‏" },
			sdg: { symbol: "SDG", name: "Sudanese Pound", format: "{} SDG" },
			sek: { symbol: "kr", name: "Swedish Krona", format: "{} kr" },
			sgd: { symbol: "$", name: "Singapore Dollar", format: "{} $" },
			sos: { symbol: "Ssh", name: "Somali Shilling", format: "{} Ssh" },
			syp: { symbol: "ل.س.‏", name: "Syrian Pound", format: "{} ل.س.‏" },
			thb: { symbol: "฿", name: "Thai Baht", format: "{} ฿" },
			tnd: { symbol: "د.ت.‏", name: "Tunisian Dinar", format: "{} د.ت.‏" },
			top: { symbol: "T$", name: "Tongan Paʻanga", format: "{} T$" },
			try: {
				symbol: "TL",
				name: "Turkish Lira",
				format: "{} TL",
			},
			ttd: { symbol: "$", name: "Trinidad and Tobago Dollar", format: "{} $" },
			twd: { symbol: "NT$", name: "New Taiwan Dollar", format: "{} NT$" },
			tzs: { symbol: "TSh", name: "Tanzanian Shilling", format: "{} TSh" },
			uah: { symbol: "₴", name: "Ukrainian Hryvnia", format: "{} ₴" },
			ugx: { symbol: "USh", name: "Ugandan Shilling", format: "{} USh" },
			uyu: { symbol: "$", name: "Uruguayan Peso", format: "{} $" },
			uzs: { symbol: "UZS", name: "Uzbekistan Som", format: "{} UZS" },
			vef: { symbol: "Bs.F.", name: "Venezuelan Bolívar", format: "{} Bs.F." },
			vnd: { symbol: "₫", name: "Vietnamese Dong", format: "{} ₫" },
			xaf: { symbol: "FCFA", name: "CFA Franc BEAC", format: "{} FCFA" },
			xof: { symbol: "CFA", name: "CFA Franc BCEAO", format: "{} CFA" },
			yer: { symbol: "ر.ي.‏", name: "Yemeni Rial", format: "{} ر.ي.‏" },
			zar: { symbol: "R", name: "South African Rand", format: "{} R" },
			zmk: { symbol: "ZK", name: "Zambian Kwacha", format: "{} ZK" },
		});
	}

	dropdownList() {
		let list = [];
		for (const id in this.data) {
			list.push([id, this.data[id].symbol + " " + this.data[id].name, [this.data[id].name, id]]);
		}
		return list;
	}

	get(currency) {
		Exception.assert(currency in this.data, "Unsupported currency '{}'.", currency);
		return this.data[currency];
	}
}

export default new Currency();
