import Jimp from "jimp";
import Svg2img from "svg2img";
import Path from "path";
import Fs from "fs";
import { fileURLToPath } from "url";

const __dirname = Path.dirname(fileURLToPath(import.meta.url));

class Flags {
	constructor() {
		this.data = Object.freeze({
			ad: {
				name: "Andorra",
				official: "Principality of Andorra",
				native: ["Andorra"],
				alt: ["AD", "Principat d'Andorra", "AND"],
				callingCode: ["376"],
				currency: ["EUR"],
				lang: ["cat"]
			},
			ae: {
				name: "United Arab Emirates",
				official: "United Arab Emirates",
				native: ["دولة الإمارات العربية المتحدة"],
				alt: ["AE", "UAE", "Emirates", "ARE"],
				callingCode: ["971"],
				currency: ["AED"],
				lang: ["ara"],
				timezone: ["UTC+04:00"]
			},
			af: {
				name: "Afghanistan",
				official: "Islamic Republic of Afghanistan",
				native: ["افغانستان", "Owganystan"],
				alt: ["AF", "Afġānistān", "AFG"],
				callingCode: ["93"],
				currency: ["AFN"],
				lang: ["prs", "pus", "tuk"],
				timezone: ["UTC+04:30"]
			},
			ag: {
				name: "Antigua and Barbuda",
				official: "Antigua and Barbuda",
				native: ["Antigua and Barbuda"],
				alt: ["AG", "ATG"],
				callingCode: ["1268"],
				currency: ["XCD"],
				lang: ["eng"]
			},
			ai: {
				name: "Anguilla",
				official: "Anguilla",
				native: ["Anguilla"],
				alt: ["AI", "AIA"],
				callingCode: ["1264"],
				currency: ["XCD"],
				lang: ["eng"]
			},
			al: {
				name: "Albania",
				official: "Republic of Albania",
				native: ["Shqipëria"],
				alt: ["AL", "Shqipëri", "Shqipëria", "Shqipnia", "ALB"],
				callingCode: ["355"],
				currency: ["ALL"],
				lang: ["sqi"],
				timezone: ["UTC+01:00"]
			},
			am: {
				name: "Armenia",
				official: "Republic of Armenia",
				native: ["Հայաստան"],
				alt: ["AM", "Hayastan", "Հայաստանի Հանրապետություն", "ARM"],
				callingCode: ["374"],
				currency: ["AMD"],
				lang: ["hye"],
				timezone: ["UTC+04:00"]
			},
			ao: {
				name: "Angola",
				official: "Republic of Angola",
				native: ["Angola"],
				alt: ["AO", "República de Angola", "ʁɛpublika de an'ɡɔla", "AGO"],
				callingCode: ["244"],
				currency: ["AOA"],
				lang: ["por"]
			},
			aq: {
				name: "Antarctica",
				official: "Antarctica",
				native: [],
				alt: ["AQ", "ATA"],
				callingCode: [],
				currency: [],
				lang: []
			},
			ar: {
				name: "Argentina",
				official: "Argentine Republic",
				native: ["Argentina"],
				alt: ["AR", "República Argentina", "ARG"],
				callingCode: ["54"],
				currency: ["ARS"],
				lang: ["grn", "spa"],
				timezone: ["UTC-03:00"]
			},
			as: {
				name: "American Samoa",
				official: "American Samoa",
				native: ["American Samoa", "Sāmoa Amelika"],
				alt: ["AS", "Amerika Sāmoa", "Amelika Sāmoa", "Sāmoa Amelika", "ASM"],
				callingCode: ["1684"],
				currency: ["USD"],
				lang: ["eng", "smo"]
			},
			at: {
				name: "Austria",
				official: "Republic of Austria",
				native: ["Österreich"],
				alt: ["AT", "Osterreich", "Oesterreich", "AUT"],
				callingCode: ["43"],
				currency: ["EUR"],
				lang: ["bar"],
				timezone: ["UTC+01:00"]
			},
			au: {
				name: "Australia",
				official: "Commonwealth of Australia",
				native: ["Australia"],
				alt: ["AU", "AUS"],
				callingCode: ["61"],
				currency: ["AUD"],
				lang: ["eng"],
				timezone: ["UTC+11:00", "UTC+10:00", "UTC+09:30", "UTC+08:00"]
			},
			aw: {
				name: "Aruba",
				official: "Aruba",
				native: ["Aruba"],
				alt: ["AW", "ABW"],
				callingCode: ["297"],
				currency: ["AWG"],
				lang: ["nld", "pap"]
			},
			ax: {
				name: "Åland Islands",
				official: "Åland Islands",
				native: ["Åland"],
				alt: ["AX", "Aaland", "Aland", "Ahvenanmaa", "ALA"],
				callingCode: ["358"],
				currency: ["EUR"],
				lang: ["swe"]
			},
			az: {
				name: "Azerbaijan",
				official: "Republic of Azerbaijan",
				native: ["Azərbaycan", "Азербайджан"],
				alt: ["AZ", "Azərbaycan Respublikası", "AZE"],
				callingCode: ["994"],
				currency: ["AZN"],
				lang: ["aze", "rus"],
				timezone: ["UTC+04:00"]
			},
			ba: {
				name: "Bosnia and Herzegovina",
				official: "Bosnia and Herzegovina",
				native: ["Bosna i Hercegovina", "Боснa и Херцеговина"],
				alt: ["BA", "Bosnia-Herzegovina", "Босна и Херцеговина", "BIH"],
				callingCode: ["387"],
				currency: ["BAM"],
				lang: ["bos", "hrv", "srp"],
				timezone: ["UTC+01:00"]
			},
			bb: {
				name: "Barbados",
				official: "Barbados",
				native: ["Barbados"],
				alt: ["BB", "BRB"],
				callingCode: ["1246"],
				currency: ["BBD"],
				lang: ["eng"]
			},
			bd: {
				name: "Bangladesh",
				official: "People's Republic of Bangladesh",
				native: ["বাংলাদেশ"],
				alt: ["BD", "Gônôprôjatôntri Bangladesh", "BGD"],
				callingCode: ["880"],
				currency: ["BDT"],
				lang: ["ben"],
				timezone: ["UTC+06:00"]
			},
			be: {
				name: "Belgium",
				official: "Kingdom of Belgium",
				native: ["Belgien", "Belgique", "België"],
				alt: [
					"BE",
					"België",
					"Belgie",
					"Belgien",
					"Belgique",
					"Koninkrijk België",
					"Royaume de Belgique",
					"Königreich Belgien",
					"BEL"
				],
				callingCode: ["32"],
				currency: ["EUR"],
				lang: ["deu", "fra", "nld"],
				timezone: ["UTC+01:00"]
			},
			bf: {
				name: "Burkina Faso",
				official: "Burkina Faso",
				native: ["Burkina Faso"],
				alt: ["BF", "BFA"],
				callingCode: ["226"],
				currency: ["XOF"],
				lang: ["fra"]
			},
			bg: {
				name: "Bulgaria",
				official: "Republic of Bulgaria",
				native: ["България"],
				alt: ["BG", "Република България", "BGR"],
				callingCode: ["359"],
				currency: ["BGN"],
				lang: ["bul"],
				timezone: ["UTC+02:00"]
			},
			bh: {
				name: "Bahrain",
				official: "Kingdom of Bahrain",
				native: ["‏البحرين"],
				alt: ["BH", "Mamlakat al-Baḥrayn", "BHR"],
				callingCode: ["973"],
				currency: ["BHD"],
				lang: ["ara"],
				timezone: ["UTC+03:00"]
			},
			bi: {
				name: "Burundi",
				official: "Republic of Burundi",
				native: ["Burundi", "Uburundi"],
				alt: ["BI", "Republika y'Uburundi", "République du Burundi", "BDI"],
				callingCode: ["257"],
				currency: ["BIF"],
				lang: ["fra", "run"]
			},
			bj: {
				name: "Benin",
				official: "Republic of Benin",
				native: ["Bénin"],
				alt: ["BJ", "République du Bénin", "BEN"],
				callingCode: ["229"],
				currency: ["XOF"],
				lang: ["fra"]
			},
			bl: {
				name: "Saint Barthélemy",
				official: "Collectivity of Saint Barthélemy",
				native: ["Saint-Barthélemy"],
				alt: ["BL", "St. Barthelemy", "Collectivité de Saint-Barthélemy", "BLM"],
				callingCode: ["590"],
				currency: ["EUR"],
				lang: ["fra"]
			},
			bm: {
				name: "Bermuda",
				official: "Bermuda",
				native: ["Bermuda"],
				alt: ["BM", "The Islands of Bermuda", "The Bermudas", "Somers Isles", "BMU"],
				callingCode: ["1441"],
				currency: ["BMD"],
				lang: ["eng"]
			},
			bn: {
				name: "Brunei",
				official: "Nation of Brunei, Abode of Peace",
				native: ["Negara Brunei Darussalam"],
				alt: ["BN", "Brunei Darussalam", "Nation of Brunei", "the Abode of Peace", "BRN"],
				callingCode: ["673"],
				currency: ["BND"],
				lang: ["msa"],
				timezone: ["UTC+08:00"]
			},
			bo: {
				name: "Bolivia",
				official: "Plurinational State of Bolivia",
				native: ["Wuliwya", "Volívia", "Buliwya", "Bolivia"],
				alt: [
					"BO",
					"Buliwya",
					"Wuliwya",
					"Bolivia, Plurinational State of",
					"Estado Plurinacional de Bolivia",
					"Buliwya Mamallaqta",
					"Wuliwya Suyu",
					"Tetã Volívia",
					"BOL"
				],
				callingCode: ["591"],
				currency: ["BOB"],
				lang: ["aym", "grn", "que", "spa"],
				timezone: ["UTC-04:00"]
			},
			bq: {
				name: "Caribbean Netherlands",
				official: "Bonaire, Sint Eustatius and Saba",
				native: ["Caribisch Nederland", "Boneiru, Sint Eustatius y Saba"],
				alt: ["BES islands", "BES"],
				callingCode: ["599"],
				currency: ["USD"],
				lang: ["eng", "nld", "pap"]
			},
			br: {
				name: "Brazil",
				official: "Federative Republic of Brazil",
				native: ["Brasil"],
				alt: ["BR", "Brasil", "República Federativa do Brasil", "BRA"],
				callingCode: ["55"],
				currency: ["BRL"],
				lang: ["por"],
				timezone: ["UTC-02:00", "UTC-03:00", "UTC-04:00", "UTC-05:00"]
			},
			bs: {
				name: "Bahamas",
				official: "Commonwealth of the Bahamas",
				native: ["Bahamas"],
				alt: ["BS", "BHS"],
				callingCode: ["1242"],
				currency: ["BSD"],
				lang: ["eng"]
			},
			bt: {
				name: "Bhutan",
				official: "Kingdom of Bhutan",
				native: ["འབྲུག་ཡུལ་"],
				alt: ["BT", "BTN"],
				callingCode: ["975"],
				currency: ["BTN", "INR"],
				lang: ["dzo"],
				timezone: ["UTC+06:00"]
			},
			bv: {
				name: "Bouvet Island",
				official: "Bouvet Island",
				native: ["Bouvetøya"],
				alt: ["BV", "Bouvetøya", "Bouvet-øya", "BVT"],
				callingCode: [],
				currency: ["NOK"],
				lang: ["nor"]
			},
			bw: {
				name: "Botswana",
				official: "Republic of Botswana",
				native: ["Botswana"],
				alt: ["BW", "Lefatshe la Botswana", "BWA"],
				callingCode: ["267"],
				currency: ["BWP"],
				lang: ["eng", "tsn"],
				timezone: ["UTC+02:00"]
			},
			by: {
				name: "Belarus",
				official: "Republic of Belarus",
				native: ["Белару́сь", "Беларусь"],
				alt: ["BY", "Bielaruś", "Белоруссия", "Республика Белоруссия", "BLR"],
				callingCode: ["375"],
				currency: ["BYN"],
				lang: ["bel", "rus"],
				timezone: ["UTC+03:00"]
			},
			bz: {
				name: "Belize",
				official: "Belize",
				native: ["Belize", "Belice"],
				alt: ["BZ", "BLZ"],
				callingCode: ["501"],
				currency: ["BZD"],
				lang: ["bjz", "eng", "spa"],
				timezone: ["UTC-06:00"]
			},
			ca: {
				name: "Canada",
				official: "Canada",
				native: ["Canada"],
				alt: ["CA", "CAN"],
				callingCode: ["1"],
				currency: ["CAD"],
				lang: ["eng", "fra"],
				timezone: ["UTC-03:30", "UTC-04:00", "UTC-05:00", "UTC-06:00", "UTC-07:00", "UTC-08:00"]
			},
			cc: {
				name: "Cocos (Keeling) Islands",
				official: "Territory of the Cocos (Keeling) Islands",
				native: ["Cocos (Keeling) Islands"],
				alt: ["CC", "Keeling Islands", "Cocos Islands", "CCK"],
				callingCode: ["61"],
				currency: ["AUD"],
				lang: ["eng"]
			},
			cd: {
				name: "DR Congo",
				official: "Democratic Republic of the Congo",
				native: [
					"RD Congo",
					"Repubilika ya Kongo Demokratiki",
					"Republiki ya Kongó Demokratiki",
					"Ditunga dia Kongu wa Mungalaata",
					"Jamhuri ya Kidemokrasia ya Kongo"
				],
				alt: ["CD", "Congo-Kinshasa", "Congo, the Democratic Republic of the", "DRC", "COD"],
				callingCode: ["243"],
				currency: ["CDF"],
				lang: ["fra", "kon", "lin", "lua", "swa"],
				timezone: ["UTC+01:00", "UTC+02:00"]
			},
			cf: {
				name: "Central African Republic",
				official: "Central African Republic",
				native: ["République centrafricaine", "Bêafrîka"],
				alt: ["CF", "République centrafricaine", "CAF"],
				callingCode: ["236"],
				currency: ["XAF"],
				lang: ["fra", "sag"]
			},
			cg: {
				name: "Republic of the Congo",
				official: "Republic of the Congo",
				native: ["République du Congo", "Repubilika ya Kongo", "Republíki ya Kongó"],
				alt: ["CG", "Congo", "Congo-Brazzaville", "COG"],
				callingCode: ["242"],
				currency: ["XAF"],
				lang: ["fra", "kon", "lin"]
			},
			ch: {
				name: "Switzerland",
				official: "Swiss Confederation",
				native: ["Suisse", "Schweiz", "Svizzera", "Svizra"],
				alt: ["CH", "Schweiz", "Suisse", "Svizzera", "Svizra", "CHE"],
				callingCode: ["41"],
				currency: ["CHF"],
				lang: ["fra", "gsw", "ita", "roh"],
				timezone: ["UTC+01:00"]
			},
			ci: {
				name: "Ivory Coast",
				official: "Republic of Côte d'Ivoire",
				native: ["Côte d'Ivoire"],
				alt: ["CI", "Côte d'Ivoire", "République de Côte d'Ivoire", "CIV"],
				callingCode: ["225"],
				currency: ["XOF"],
				lang: ["fra"],
				timezone: ["UTC+00:00"]
			},
			ck: {
				name: "Cook Islands",
				official: "Cook Islands",
				native: ["Cook Islands", "Kūki 'Āirani"],
				alt: ["CK", "Kūki 'Āirani", "COK"],
				callingCode: ["682"],
				currency: ["NZD", "CKD"],
				lang: ["eng", "rar"]
			},
			cl: {
				name: "Chile",
				official: "Republic of Chile",
				native: ["Chile"],
				alt: ["CL", "República de Chile", "CHL"],
				callingCode: ["56"],
				currency: ["CLP"],
				lang: ["spa"],
				timezone: ["UTC-04:00", "UTC-05:00"]
			},
			cm: {
				name: "Cameroon",
				official: "Republic of Cameroon",
				native: ["Cameroon", "Cameroun"],
				alt: ["CM", "République du Cameroun", "CMR"],
				callingCode: ["237"],
				currency: ["XAF"],
				lang: ["eng", "fra"],
				timezone: ["UTC+01:00"]
			},
			cn: {
				name: "China",
				official: "People's Republic of China",
				native: ["中国"],
				alt: ["CN", "Zhōngguó", "Zhongguo", "Zhonghua", "中华人民共和国", "Zhōnghuá Rénmín Gònghéguó", "CHN"],
				callingCode: ["86"],
				currency: ["CNY"],
				lang: ["zho"],
				timezone: ["UTC+08:00", "UTC+06:00"]
			},
			co: {
				name: "Colombia",
				official: "Republic of Colombia",
				native: ["Colombia"],
				alt: ["CO", "República de Colombia", "COL"],
				callingCode: ["57"],
				currency: ["COP"],
				lang: ["spa"],
				timezone: ["UTC-05:00"]
			},
			cr: {
				name: "Costa Rica",
				official: "Republic of Costa Rica",
				native: ["Costa Rica"],
				alt: ["CR", "República de Costa Rica", "CRI"],
				callingCode: ["506"],
				currency: ["CRC"],
				lang: ["spa"],
				timezone: ["UTC-06:00"]
			},
			cu: {
				name: "Cuba",
				official: "Republic of Cuba",
				native: ["Cuba"],
				alt: ["CU", "República de Cuba", "CUB"],
				callingCode: ["53"],
				currency: ["CUC", "CUP"],
				lang: ["spa"],
				timezone: ["UTC-05:00"]
			},
			cv: {
				name: "Cape Verde",
				official: "Republic of Cabo Verde",
				native: ["Cabo Verde"],
				alt: ["CV", "República de Cabo Verde", "CPV"],
				callingCode: ["238"],
				currency: ["CVE"],
				lang: ["por"]
			},
			cw: {
				name: "Curaçao",
				official: "Country of Curaçao",
				native: ["Curaçao", "Pais Kòrsou"],
				alt: ["CW", "Curacao", "Kòrsou", "Land Curaçao", "Pais Kòrsou", "CUW"],
				callingCode: ["5999"],
				currency: ["ANG"],
				lang: ["eng", "nld", "pap"]
			},
			cx: {
				name: "Christmas Island",
				official: "Territory of Christmas Island",
				native: ["Christmas Island"],
				alt: ["CX", "CXR"],
				callingCode: ["61"],
				currency: ["AUD"],
				lang: ["eng"]
			},
			cy: {
				name: "Cyprus",
				official: "Republic of Cyprus",
				native: ["Κύπρος", "Kıbrıs"],
				alt: ["CY", "Kýpros", "Kıbrıs", "Κυπριακή Δημοκρατία", "Kıbrıs Cumhuriyeti", "CYP"],
				callingCode: ["357"],
				currency: ["EUR"],
				lang: ["ell", "tur"]
			},
			cz: {
				name: "Czechia",
				official: "Czech Republic",
				native: ["Česko"],
				alt: ["CZ", "Česká republika", "Česko", "CZE"],
				callingCode: ["420"],
				currency: ["CZK"],
				lang: ["ces", "slk"],
				timezone: ["UTC+01:00"]
			},
			de: {
				name: "Germany",
				official: "Federal Republic of Germany",
				native: ["Deutschland"],
				alt: ["DE", "Bundesrepublik Deutschland", "DEU"],
				callingCode: ["49"],
				currency: ["EUR"],
				lang: ["deu"],
				timezone: ["UTC+01:00"]
			},
			dj: {
				name: "Djibouti",
				official: "Republic of Djibouti",
				native: ["جيبوتي‎", "Djibouti"],
				alt: ["DJ", "Jabuuti", "Gabuuti", "République de Djibouti", "Gabuutih Ummuuno", "Jamhuuriyadda Jabuuti", "DJI"],
				callingCode: ["253"],
				currency: ["DJF"],
				lang: ["ara", "fra"],
				timezone: ["UTC+03:00"]
			},
			dk: {
				name: "Denmark",
				official: "Kingdom of Denmark",
				native: ["Danmark"],
				alt: ["DK", "Danmark", "Kongeriget Danmark", "DNK"],
				callingCode: ["45"],
				currency: ["DKK"],
				lang: ["dan"],
				timezone: ["UTC+01:00"]
			},
			dm: {
				name: "Dominica",
				official: "Commonwealth of Dominica",
				native: ["Dominica"],
				alt: ["DM", "Dominique", "Wai‘tu kubuli", "DMA"],
				callingCode: ["1767"],
				currency: ["XCD"],
				lang: ["eng"]
			},
			do: {
				name: "Dominican Republic",
				official: "Dominican Republic",
				native: ["República Dominicana"],
				alt: ["DO", "DOM"],
				callingCode: ["1809", "1829", "1849"],
				currency: ["DOP"],
				lang: ["spa"],
				timezone: ["UTC-04:00"]
			},
			dz: {
				name: "Algeria",
				official: "People's Democratic Republic of Algeria",
				native: ["الجزائر"],
				alt: ["DZ", "Dzayer", "Algérie", "DZA"],
				callingCode: ["213"],
				currency: ["DZD"],
				lang: ["ara"],
				timezone: ["UTC+01:00"]
			},
			ec: {
				name: "Ecuador",
				official: "Republic of Ecuador",
				native: ["Ecuador"],
				alt: ["EC", "República del Ecuador", "ECU"],
				callingCode: ["593"],
				currency: ["USD"],
				lang: ["spa"],
				timezone: ["UTC-05:00", "UTC-06:00"]
			},
			ee: {
				name: "Estonia",
				official: "Republic of Estonia",
				native: ["Eesti"],
				alt: ["EE", "Eesti", "Eesti Vabariik", "EST"],
				callingCode: ["372"],
				currency: ["EUR"],
				lang: ["est"],
				timezone: ["UTC+02:00"]
			},
			eg: {
				name: "Egypt",
				official: "Arab Republic of Egypt",
				native: ["مصر"],
				alt: ["EG", "EGY"],
				callingCode: ["20"],
				currency: ["EGP"],
				lang: ["ara"],
				timezone: ["UTC+02:00"]
			},
			eh: {
				name: "Western Sahara",
				official: "Sahrawi Arab Democratic Republic",
				native: ["Western Sahara", "الصحراء الغربية", "Sahara Occidental"],
				alt: ["EH", "Taneẓroft Tutrimt", "ESH"],
				callingCode: ["212"],
				currency: ["MAD", "DZD", "MRO"],
				lang: ["ber", "mey", "spa"]
			},
			er: {
				name: "Eritrea",
				official: "State of Eritrea",
				native: ["إرتريا‎", "Eritrea", "ኤርትራ"],
				alt: ["ER", "ሃገረ ኤርትራ", "Dawlat Iritriyá", "ʾErtrā", "Iritriyā", "ERI"],
				callingCode: ["291"],
				currency: ["ERN"],
				lang: ["ara", "eng", "tir"],
				timezone: ["UTC+03:00"]
			},
			es: {
				name: "Spain",
				official: "Kingdom of Spain",
				native: ["España"],
				alt: ["ES", "Reino de España", "ESP"],
				callingCode: ["34"],
				currency: ["EUR"],
				lang: ["spa"],
				timezone: ["UTC+01:00", "UTC+00:00"]
			},
			et: {
				name: "Ethiopia",
				official: "Federal Democratic Republic of Ethiopia",
				native: ["ኢትዮጵያ"],
				alt: ["ET", "ʾĪtyōṗṗyā", "የኢትዮጵያ ፌዴራላዊ ዲሞክራሲያዊ ሪፐብሊክ", "ETH"],
				callingCode: ["251"],
				currency: ["ETB"],
				lang: ["amh"],
				timezone: ["UTC+03:00"]
			},
			fi: {
				name: "Finland",
				official: "Republic of Finland",
				native: ["Suomi", "Finland"],
				alt: ["FI", "Suomi", "Suomen tasavalta", "Republiken Finland", "FIN"],
				callingCode: ["358"],
				currency: ["EUR"],
				lang: ["fin", "swe"],
				timezone: ["UTC+02:00"]
			},
			fj: {
				name: "Fiji",
				official: "Republic of Fiji",
				native: ["Fiji", "Viti", "फिजी"],
				alt: ["FJ", "Viti", "Matanitu ko Viti", "Fijī Gaṇarājya", "FJI"],
				callingCode: ["679"],
				currency: ["FJD"],
				lang: ["eng", "fij", "hif"]
			},
			fk: {
				name: "Falkland Islands",
				official: "Falkland Islands",
				native: ["Falkland Islands"],
				alt: ["FK", "Islas Malvinas", "Falkland Islands (Malvinas)", "FLK"],
				callingCode: ["500"],
				currency: ["FKP"],
				lang: ["eng"]
			},
			fm: {
				name: "Micronesia",
				official: "Federated States of Micronesia",
				native: ["Micronesia"],
				alt: ["FM", "Micronesia, Federated States of", "FSM"],
				callingCode: ["691"],
				currency: ["USD"],
				lang: ["eng"]
			},
			fo: {
				name: "Faroe Islands",
				official: "Faroe Islands",
				native: ["Færøerne", "Føroyar"],
				alt: ["FO", "Føroyar", "Færøerne", "FRO"],
				callingCode: ["298"],
				currency: ["DKK"],
				lang: ["dan", "fao"],
				timezone: ["UTC+00:00"]
			},
			fr: {
				name: "France",
				official: "French Republic",
				native: ["France"],
				alt: ["FR", "République française", "FRA"],
				callingCode: ["33"],
				currency: ["EUR"],
				lang: ["fra"],
				timezone: ["UTC+01:00"]
			},
			ga: {
				name: "Gabon",
				official: "Gabonese Republic",
				native: ["Gabon"],
				alt: ["GA", "République Gabonaise", "GAB"],
				callingCode: ["241"],
				currency: ["XAF"],
				lang: ["fra"]
			},
			gb: {
				name: "United Kingdom",
				official: "United Kingdom of Great Britain and Northern Ireland",
				native: ["United Kingdom"],
				alt: ["GB", "UK", "Great Britain", "GBR", "england"],
				callingCode: ["44"],
				currency: ["GBP"],
				lang: ["eng"],
				timezone: ["UTC+00:00"]
			},
			gd: {
				name: "Grenada",
				official: "Grenada",
				native: ["Grenada"],
				alt: ["GD", "GRD"],
				callingCode: ["1473"],
				currency: ["XCD"],
				lang: ["eng"]
			},
			ge: {
				name: "Georgia",
				official: "Georgia",
				native: ["საქართველო"],
				alt: ["GE", "Sakartvelo", "GEO"],
				callingCode: ["995"],
				currency: ["GEL"],
				lang: ["kat"],
				timezone: ["UTC+04:00"]
			},
			gf: {
				name: "French Guiana",
				official: "Guiana",
				native: ["Guyane française"],
				alt: ["GF", "Guyane", "GUF"],
				callingCode: ["594"],
				currency: ["EUR"],
				lang: ["fra"]
			},
			gg: {
				name: "Guernsey",
				official: "Bailiwick of Guernsey",
				native: ["Guernsey", "Guernesey", "Dgèrnésiais"],
				alt: ["GG", "Bailliage de Guernesey", "GGY"],
				callingCode: ["44"],
				currency: ["GBP"],
				lang: ["eng", "fra", "nfr"]
			},
			gh: {
				name: "Ghana",
				official: "Republic of Ghana",
				native: ["Ghana"],
				alt: ["GH", "GHA"],
				callingCode: ["233"],
				currency: ["GHS"],
				lang: ["eng"]
			},
			gi: {
				name: "Gibraltar",
				official: "Gibraltar",
				native: ["Gibraltar"],
				alt: ["GI", "GIB"],
				callingCode: ["350"],
				currency: ["GIP"],
				lang: ["eng"]
			},
			gl: {
				name: "Greenland",
				official: "Greenland",
				native: ["Kalaallit Nunaat"],
				alt: ["GL", "Grønland", "GRL"],
				callingCode: ["299"],
				currency: ["DKK"],
				lang: ["kal"],
				timezone: ["UTC-03:00", "UTC", "UTC-01:00", "UTC-04:00"]
			},
			gm: {
				name: "Gambia",
				official: "Republic of the Gambia",
				native: ["Gambia"],
				alt: ["GM", "GMB"],
				callingCode: ["220"],
				currency: ["GMD"],
				lang: ["eng"]
			},
			gn: {
				name: "Guinea",
				official: "Republic of Guinea",
				native: ["Guinée"],
				alt: ["GN", "République de Guinée", "GIN"],
				callingCode: ["224"],
				currency: ["GNF"],
				lang: ["fra"]
			},
			gp: {
				name: "Guadeloupe",
				official: "Guadeloupe",
				native: ["Guadeloupe"],
				alt: ["GP", "Gwadloup", "GLP"],
				callingCode: ["590"],
				currency: ["EUR"],
				lang: ["fra"]
			},
			gq: {
				name: "Equatorial Guinea",
				official: "Republic of Equatorial Guinea",
				native: ["Guinée équatoriale", "Guiné Equatorial", "Guinea Ecuatorial"],
				alt: [
					"GQ",
					"República de Guinea Ecuatorial",
					"République de Guinée équatoriale",
					"República da Guiné Equatorial",
					"GNQ"
				],
				callingCode: ["240"],
				currency: ["XAF"],
				lang: ["fra", "por", "spa"]
			},
			gr: {
				name: "Greece",
				official: "Hellenic Republic",
				native: ["Ελλάδα"],
				alt: ["GR", "Elláda", "Ελληνική Δημοκρατία", "GRC"],
				callingCode: ["30"],
				currency: ["EUR"],
				lang: ["ell"],
				timezone: ["UTC+02:00"]
			},
			gs: {
				name: "South Georgia",
				official: "South Georgia and the South Sandwich Islands",
				native: ["South Georgia"],
				alt: ["GS", "SGS"],
				callingCode: ["500"],
				currency: ["GBP"],
				lang: ["eng"]
			},
			gt: {
				name: "Guatemala",
				official: "Republic of Guatemala",
				native: ["Guatemala"],
				alt: ["GT", "GTM"],
				callingCode: ["502"],
				currency: ["GTQ"],
				lang: ["spa"],
				timezone: ["UTC-06:00"]
			},
			gu: {
				name: "Guam",
				official: "Guam",
				native: ["Guåhån", "Guam"],
				alt: ["GU", "Guåhån", "GUM"],
				callingCode: ["1671"],
				currency: ["USD"],
				lang: ["cha", "eng", "spa"]
			},
			gw: {
				name: "Guinea-Bissau",
				official: "Republic of Guinea-Bissau",
				native: ["Guiné-Bissau"],
				alt: ["GW", "República da Guiné-Bissau", "GNB"],
				callingCode: ["245"],
				currency: ["XOF"],
				lang: ["por", "pov"]
			},
			gy: {
				name: "Guyana",
				official: "Co-operative Republic of Guyana",
				native: ["Guyana"],
				alt: ["GY", "GUY"],
				callingCode: ["592"],
				currency: ["GYD"],
				lang: ["eng"]
			},
			hk: {
				name: "Hong Kong",
				official: "Hong Kong Special Administrative Region of the People's Republic of China",
				native: ["Hong Kong", "香港"],
				alt: ["HK", "HKG"],
				callingCode: ["852"],
				currency: ["HKD"],
				lang: ["eng", "zho"],
				timezone: ["UTC+08:00"]
			},
			hm: {
				name: "Heard Island and McDonald Islands",
				official: "Heard Island and McDonald Islands",
				native: ["Heard Island and McDonald Islands"],
				alt: ["HM", "HMD"],
				callingCode: [],
				currency: ["AUD"],
				lang: ["eng"]
			},
			hn: {
				name: "Honduras",
				official: "Republic of Honduras",
				native: ["Honduras"],
				alt: ["HN", "República de Honduras", "HND"],
				callingCode: ["504"],
				currency: ["HNL"],
				lang: ["spa"],
				timezone: ["UTC-06:00"]
			},
			hr: {
				name: "Croatia",
				official: "Republic of Croatia",
				native: ["Hrvatska"],
				alt: ["HR", "Hrvatska", "Republika Hrvatska", "HRV"],
				callingCode: ["385"],
				currency: ["HRK"],
				lang: ["hrv"],
				timezone: ["UTC+01:00"]
			},
			ht: {
				name: "Haiti",
				official: "Republic of Haiti",
				native: ["Haïti", "Ayiti"],
				alt: ["HT", "République d'Haïti", "Repiblik Ayiti", "HTI"],
				callingCode: ["509"],
				currency: ["HTG", "USD"],
				lang: ["fra", "hat"],
				timezone: ["UTC-05:00"]
			},
			hu: {
				name: "Hungary",
				official: "Hungary",
				native: ["Magyarország"],
				alt: ["HU", "HUN"],
				callingCode: ["36"],
				currency: ["HUF"],
				lang: ["hun"],
				timezone: ["UTC+01:00"]
			},
			id: {
				name: "Indonesia",
				official: "Republic of Indonesia",
				native: ["Indonesia"],
				alt: ["ID", "Republik Indonesia", "IDN"],
				callingCode: ["62"],
				currency: ["IDR"],
				lang: ["ind"],
				timezone: ["UTC+07:00", "UTC+08:00", "UTC+09:00"]
			},
			ie: {
				name: "Ireland",
				official: "Republic of Ireland",
				native: ["Ireland", "Éire"],
				alt: ["IE", "Éire", "Poblacht na hÉireann", "IRL"],
				callingCode: ["353"],
				currency: ["EUR"],
				lang: ["eng", "gle"],
				timezone: ["UTC+00:00"]
			},
			il: {
				name: "Israel",
				official: "State of Israel",
				native: ["إسرائيل", "ישראל"],
				alt: ["IL", "Medīnat Yisrā'el", "ISR"],
				callingCode: ["972"],
				currency: ["ILS"],
				lang: ["ara", "heb"],
				timezone: ["UTC+02:00"]
			},
			im: {
				name: "Isle of Man",
				official: "Isle of Man",
				native: ["Isle of Man", "Mannin"],
				alt: ["IM", "Ellan Vannin", "Mann", "Mannin", "IMN"],
				callingCode: ["44"],
				currency: ["GBP"],
				lang: ["eng", "glv"]
			},
			in: {
				name: "India",
				official: "Republic of India",
				native: ["India", "भारत", "இந்தியா"],
				alt: ["IN", "Bhārat", "Bharat Ganrajya", "இந்தியா", "IND"],
				callingCode: ["91"],
				currency: ["INR"],
				lang: ["eng", "hin", "tam"],
				timezone: ["UTC+05:30"]
			},
			io: {
				name: "British Indian Ocean Territory",
				official: "British Indian Ocean Territory",
				native: ["British Indian Ocean Territory"],
				alt: ["IO", "IOT"],
				callingCode: ["246"],
				currency: ["USD"],
				lang: ["eng"]
			},
			iq: {
				name: "Iraq",
				official: "Republic of Iraq",
				native: ["العراق", "ܩܘܼܛܢܵܐ", "کۆماری"],
				alt: ["IQ", "Jumhūriyyat al-‘Irāq", "IRQ"],
				callingCode: ["964"],
				currency: ["IQD"],
				lang: ["ara", "arc", "ckb"],
				timezone: ["UTC+03:00"]
			},
			ir: {
				name: "Iran",
				official: "Islamic Republic of Iran",
				native: ["ایران"],
				alt: ["IR", "Iran, Islamic Republic of", "Jomhuri-ye Eslāmi-ye Irān", "IRN"],
				callingCode: ["98"],
				currency: ["IRR"],
				lang: ["fas"],
				timezone: ["UTC+03:30"]
			},
			is: {
				name: "Iceland",
				official: "Iceland",
				native: ["Ísland"],
				alt: ["IS", "Island", "Republic of Iceland", "Lýðveldið Ísland", "ISL"],
				callingCode: ["354"],
				currency: ["ISK"],
				lang: ["isl"],
				timezone: ["UTC+00:00"]
			},
			it: {
				name: "Italy",
				official: "Italian Republic",
				native: ["Italia"],
				alt: ["IT", "Repubblica italiana", "ITA"],
				callingCode: ["39"],
				currency: ["EUR"],
				lang: ["ita"],
				timezone: ["UTC+01:00"]
			},
			je: {
				name: "Jersey",
				official: "Bailiwick of Jersey",
				native: ["Jersey", "Jèrri"],
				alt: ["JE", "Bailliage de Jersey", "Bailliage dé Jèrri", "JEY"],
				callingCode: ["44"],
				currency: ["GBP"],
				lang: ["eng", "fra", "nrf"]
			},
			jm: {
				name: "Jamaica",
				official: "Jamaica",
				native: ["Jamaica"],
				alt: ["JM", "JAM"],
				callingCode: ["1876"],
				currency: ["JMD"],
				lang: ["eng", "jam"],
				timezone: ["UTC-05:00"]
			},
			jo: {
				name: "Jordan",
				official: "Hashemite Kingdom of Jordan",
				native: ["الأردن"],
				alt: ["JO", "al-Mamlakah al-Urdunīyah al-Hāshimīyah", "JOR"],
				callingCode: ["962"],
				currency: ["JOD"],
				lang: ["ara"],
				timezone: ["UTC+02:00"]
			},
			jp: {
				name: "Japan",
				official: "Japan",
				native: ["日本"],
				alt: ["JP", "Nippon", "Nihon", "JPN"],
				callingCode: ["81"],
				currency: ["JPY"],
				lang: ["jpn"],
				timezone: ["UTC+09:00"]
			},
			ke: {
				name: "Kenya",
				official: "Republic of Kenya",
				native: ["Kenya"],
				alt: ["KE", "Jamhuri ya Kenya", "KEN"],
				callingCode: ["254"],
				currency: ["KES"],
				lang: ["eng", "swa"],
				timezone: ["UTC+03:00"]
			},
			kg: {
				name: "Kyrgyzstan",
				official: "Kyrgyz Republic",
				native: ["Кыргызстан", "Киргизия"],
				alt: ["KG", "Киргизия", "Кыргыз Республикасы", "Kyrgyz Respublikasy", "KGZ"],
				callingCode: ["996"],
				currency: ["KGS"],
				lang: ["kir", "rus"],
				timezone: ["UTC+06:00"]
			},
			kh: {
				name: "Cambodia",
				official: "Kingdom of Cambodia",
				native: ["Kâmpŭchéa"],
				alt: ["KH", "KHM"],
				callingCode: ["855"],
				currency: ["KHR"],
				lang: ["khm"],
				timezone: ["UTC+07:00"]
			},
			ki: {
				name: "Kiribati",
				official: "Independent and Sovereign Republic of Kiribati",
				native: ["Kiribati"],
				alt: ["KI", "Republic of Kiribati", "Ribaberiki Kiribati", "KIR"],
				callingCode: ["686"],
				currency: ["AUD"],
				lang: ["eng", "gil"]
			},
			km: {
				name: "Comoros",
				official: "Union of the Comoros",
				native: ["القمر‎", "Comores", "Komori"],
				alt: ["KM", "Union des Comores", "Udzima wa Komori", "al-Ittiḥād al-Qumurī", "COM"],
				callingCode: ["269"],
				currency: ["KMF"],
				lang: ["ara", "fra", "zdj"]
			},
			kn: {
				name: "Saint Kitts and Nevis",
				official: "Federation of Saint Christopher and Nevisa",
				native: ["Saint Kitts and Nevis"],
				alt: ["KN", "Federation of Saint Christopher and Nevis", "KNA"],
				callingCode: ["1869"],
				currency: ["XCD"],
				lang: ["eng"]
			},
			kp: {
				name: "North Korea",
				official: "Democratic People's Republic of Korea",
				native: ["조선"],
				alt: [
					"KP",
					"조선민주주의인민공화국",
					"Chosŏn Minjujuŭi Inmin Konghwaguk",
					"Korea, Democratic People's Republic of",
					"북한",
					"북조선",
					"PRK"
				],
				callingCode: ["850"],
				currency: ["KPW"],
				lang: ["kor"]
			},
			kr: {
				name: "South Korea",
				official: "Republic of Korea",
				native: ["한국"],
				alt: ["KR", "Korea, Republic of", "남한", "남조선", "KOR"],
				callingCode: ["82"],
				currency: ["KRW"],
				lang: ["kor"],
				timezone: ["UTC+09:00"]
			},
			kw: {
				name: "Kuwait",
				official: "State of Kuwait",
				native: ["الكويت"],
				alt: ["KW", "Dawlat al-Kuwait", "KWT"],
				callingCode: ["965"],
				currency: ["KWD"],
				lang: ["ara"],
				timezone: ["UTC+03:00"]
			},
			ky: {
				name: "Cayman Islands",
				official: "Cayman Islands",
				native: ["Cayman Islands"],
				alt: ["KY", "CYM"],
				callingCode: ["1345"],
				currency: ["KYD"],
				lang: ["eng"]
			},
			kz: {
				name: "Kazakhstan",
				official: "Republic of Kazakhstan",
				native: ["Қазақстан", "Казахстан"],
				alt: [
					"KZ",
					"Qazaqstan",
					"Казахстан",
					"Қазақстан Республикасы",
					"Qazaqstan Respublïkası",
					"Республика Казахстан",
					"Respublika Kazakhstan",
					"KAZ"
				],
				callingCode: ["7"],
				currency: ["KZT"],
				lang: ["kaz", "rus"],
				timezone: ["UTC+06:00", "UTC+05:00"]
			},
			la: {
				name: "Laos",
				official: "Lao People's Democratic Republic",
				native: ["ສປປລາວ"],
				alt: ["LA", "Lao", "Sathalanalat Paxathipatai Paxaxon Lao", "LAO"],
				callingCode: ["856"],
				currency: ["LAK"],
				lang: ["lao"],
				timezone: ["UTC+07:00"]
			},
			lb: {
				name: "Lebanon",
				official: "Lebanese Republic",
				native: ["لبنان", "Liban"],
				alt: ["LB", "Al-Jumhūrīyah Al-Libnānīyah", "LBN"],
				callingCode: ["961"],
				currency: ["LBP"],
				lang: ["ara", "fra"],
				timezone: ["UTC+02:00"]
			},
			lc: {
				name: "Saint Lucia",
				official: "Saint Lucia",
				native: ["Saint Lucia"],
				alt: ["LC", "LCA"],
				callingCode: ["1758"],
				currency: ["XCD"],
				lang: ["eng"]
			},
			li: {
				name: "Liechtenstein",
				official: "Principality of Liechtenstein",
				native: ["Liechtenstein"],
				alt: ["LI", "Fürstentum Liechtenstein", "LIE"],
				callingCode: ["423"],
				currency: ["CHF"],
				lang: ["deu"],
				timezone: ["UTC+01:00"]
			},
			lk: {
				name: "Sri Lanka",
				official: "Democratic Socialist Republic of Sri Lanka",
				native: ["ශ්‍රී ලංකාව", "இலங்கை"],
				alt: ["LK", "ilaṅkai", "LKA"],
				callingCode: ["94"],
				currency: ["LKR"],
				lang: ["sin", "tam"],
				timezone: ["UTC+05:30"]
			},
			lr: {
				name: "Liberia",
				official: "Republic of Liberia",
				native: ["Liberia"],
				alt: ["LR", "LBR"],
				callingCode: ["231"],
				currency: ["LRD"],
				lang: ["eng"]
			},
			ls: {
				name: "Lesotho",
				official: "Kingdom of Lesotho",
				native: ["Lesotho"],
				alt: ["LS", "Muso oa Lesotho", "LSO"],
				callingCode: ["266"],
				currency: ["LSL", "ZAR"],
				lang: ["eng", "sot"]
			},
			lt: {
				name: "Lithuania",
				official: "Republic of Lithuania",
				native: ["Lietuva"],
				alt: ["LT", "Lietuvos Respublika", "LTU"],
				callingCode: ["370"],
				currency: ["EUR"],
				lang: ["lit"],
				timezone: ["UTC+02:00"]
			},
			lu: {
				name: "Luxembourg",
				official: "Grand Duchy of Luxembourg",
				native: ["Luxemburg", "Luxembourg", "Lëtzebuerg"],
				alt: ["LU", "Grand-Duché de Luxembourg", "Großherzogtum Luxemburg", "Groussherzogtum Lëtzebuerg", "LUX"],
				callingCode: ["352"],
				currency: ["EUR"],
				lang: ["deu", "fra", "ltz"],
				timezone: ["UTC+01:00"]
			},
			lv: {
				name: "Latvia",
				official: "Republic of Latvia",
				native: ["Latvija"],
				alt: ["LV", "Latvijas Republika", "LVA"],
				callingCode: ["371"],
				currency: ["EUR"],
				lang: ["lav"],
				timezone: ["UTC+02:00"]
			},
			ly: {
				name: "Libya",
				official: "State of Libya",
				native: ["‏ليبيا"],
				alt: ["LY", "Dawlat Libya", "LBY"],
				callingCode: ["218"],
				currency: ["LYD"],
				lang: ["ara"],
				timezone: ["UTC+02:00"]
			},
			ma: {
				name: "Morocco",
				official: "Kingdom of Morocco",
				native: ["المغرب", "ⵍⵎⴰⵖⵔⵉⴱ"],
				alt: ["MA", "Al-Mamlakah al-Maġribiyah", "MAR"],
				callingCode: ["212"],
				currency: ["MAD"],
				lang: ["ara", "ber"],
				timezone: ["UTC+00:00"]
			},
			mc: {
				name: "Monaco",
				official: "Principality of Monaco",
				native: ["Monaco"],
				alt: ["MC", "Principauté de Monaco", "MCO"],
				callingCode: ["377"],
				currency: ["EUR"],
				lang: ["fra"],
				timezone: ["UTC+01:00"]
			},
			md: {
				name: "Moldova",
				official: "Republic of Moldova",
				native: ["Moldova"],
				alt: ["MD", "Moldova, Republic of", "Republica Moldova", "MDA"],
				callingCode: ["373"],
				currency: ["MDL"],
				lang: ["ron"],
				timezone: ["UTC+02:00"]
			},
			me: {
				name: "Montenegro",
				official: "Montenegro",
				native: ["Црна Гора"],
				alt: ["ME", "Crna Gora", "MNE"],
				callingCode: ["382"],
				currency: ["EUR"],
				lang: ["srp"],
				timezone: ["UTC+01:00"]
			},
			mf: {
				name: "Saint Martin",
				official: "Saint Martin",
				native: ["Saint-Martin"],
				alt: [
					"MF",
					"Collectivity of Saint Martin",
					"Collectivité de Saint-Martin",
					"Saint Martin (French part)",
					"MAF"
				],
				callingCode: ["590"],
				currency: ["EUR"],
				lang: ["fra"]
			},
			mg: {
				name: "Madagascar",
				official: "Republic of Madagascar",
				native: ["Madagascar", "Madagasikara"],
				alt: ["MG", "Repoblikan'i Madagasikara", "République de Madagascar", "MDG"],
				callingCode: ["261"],
				currency: ["MGA"],
				lang: ["fra", "mlg"]
			},
			mh: {
				name: "Marshall Islands",
				official: "Republic of the Marshall Islands",
				native: ["Marshall Islands", "M̧ajeļ"],
				alt: ["MH", "Aolepān Aorōkin M̧ajeļ", "MHL"],
				callingCode: ["692"],
				currency: ["USD"],
				lang: ["eng", "mah"]
			},
			mk: {
				name: "Macedonia",
				official: "Republic of Macedonia",
				native: ["Македонија"],
				alt: ["MK", "Macedonia, the Former Yugoslav Republic of", "Република Македонија", "MKD"],
				callingCode: ["389"],
				currency: ["MKD"],
				lang: ["mkd"],
				timezone: ["UTC+01:00"]
			},
			ml: {
				name: "Mali",
				official: "Republic of Mali",
				native: ["Mali"],
				alt: ["ML", "République du Mali", "MLI"],
				callingCode: ["223"],
				currency: ["XOF"],
				lang: ["fra"],
				timezone: ["UTC+00:00"]
			},
			mm: {
				name: "Myanmar",
				official: "Republic of the Union of Myanmar",
				native: ["မြန်မာ"],
				alt: ["MM", "Burma", "Pyidaunzu Thanmăda Myăma Nainngandaw", "MMR"],
				callingCode: ["95"],
				currency: ["MMK"],
				lang: ["mya"],
				timezone: ["UTC+06:30"]
			},
			mn: {
				name: "Mongolia",
				official: "Mongolia",
				native: ["Монгол улс"],
				alt: ["MN", "MNG"],
				callingCode: ["976"],
				currency: ["MNT"],
				lang: ["mon"],
				timezone: ["UTC+08:00"]
			},
			mo: {
				name: "Macau",
				official: "Macao Special Administrative Region of the People's Republic of China",
				native: ["Macau", "澳门"],
				alt: [
					"MO",
					"澳门",
					"Macao",
					"中華人民共和國澳門特別行政區",
					"Região Administrativa Especial de Macau da República Popular da China",
					"MAC"
				],
				callingCode: ["853"],
				currency: ["MOP"],
				lang: ["por", "zho"],
				timezone: ["UTC+08:00"]
			},
			mp: {
				name: "Northern Mariana Islands",
				official: "Commonwealth of the Northern Mariana Islands",
				native: ["Northern Mariana Islands", "Na Islas Mariånas"],
				alt: ["MP", "Sankattan Siha Na Islas Mariånas", "MNP"],
				callingCode: ["1670"],
				currency: ["USD"],
				lang: ["cal", "cha", "eng"]
			},
			mq: {
				name: "Martinique",
				official: "Martinique",
				native: ["Martinique"],
				alt: ["MQ", "MTQ"],
				callingCode: ["596"],
				currency: ["EUR"],
				lang: ["fra"]
			},
			mr: {
				name: "Mauritania",
				official: "Islamic Republic of Mauritania",
				native: ["موريتانيا"],
				alt: ["MR", "al-Jumhūriyyah al-ʾIslāmiyyah al-Mūrītāniyyah", "MRT"],
				callingCode: ["222"],
				currency: ["MRO"],
				lang: ["ara"]
			},
			ms: {
				name: "Montserrat",
				official: "Montserrat",
				native: ["Montserrat"],
				alt: ["MS", "MSR"],
				callingCode: ["1664"],
				currency: ["XCD"],
				lang: ["eng"]
			},
			mt: {
				name: "Malta",
				official: "Republic of Malta",
				native: ["Malta"],
				alt: ["MT", "Repubblika ta' Malta", "MLT"],
				callingCode: ["356"],
				currency: ["EUR"],
				lang: ["eng", "mlt"],
				timezone: ["UTC+01:00"]
			},
			mu: {
				name: "Mauritius",
				official: "Republic of Mauritius",
				native: ["Mauritius", "Maurice", "Moris"],
				alt: ["MU", "République de Maurice", "MUS"],
				callingCode: ["230"],
				currency: ["MUR"],
				lang: ["eng", "fra", "mfe"]
			},
			mv: {
				name: "Maldives",
				official: "Republic of the Maldives",
				native: ["ދިވެހިރާއްޖޭގެ"],
				alt: ["MV", "Maldive Islands", "Dhivehi Raajjeyge Jumhooriyya", "MDV"],
				callingCode: ["960"],
				currency: ["MVR"],
				lang: ["div"],
				timezone: ["UTC+05:00"]
			},
			mw: {
				name: "Malawi",
				official: "Republic of Malawi",
				native: ["Malawi", "Malaŵi"],
				alt: ["MW", "MWI"],
				callingCode: ["265"],
				currency: ["MWK"],
				lang: ["eng", "nya"]
			},
			mx: {
				name: "Mexico",
				official: "United Mexican States",
				native: ["México"],
				alt: ["MX", "Mexicanos", "Estados Unidos Mexicanos", "MEX"],
				callingCode: ["52"],
				currency: ["MXN"],
				lang: ["spa"],
				timezone: ["UTC-06:00", "UTC-05:00", "UTC-07:00", "UTC-08:00"]
			},
			my: {
				name: "Malaysia",
				official: "Malaysia",
				native: ["Malaysia", "مليسيا"],
				alt: ["MY", "MYS"],
				callingCode: ["60"],
				currency: ["MYR"],
				lang: ["eng", "msa"],
				timezone: ["UTC+08:00"]
			},
			mz: {
				name: "Mozambique",
				official: "Republic of Mozambique",
				native: ["Moçambique"],
				alt: ["MZ", "República de Moçambique", "MOZ"],
				callingCode: ["258"],
				currency: ["MZN"],
				lang: ["por"]
			},
			na: {
				name: "Namibia",
				official: "Republic of Namibia",
				native: ["Namibië", "Namibia"],
				alt: ["NA", "Namibië", "NAM"],
				callingCode: ["264"],
				currency: ["NAD", "ZAR"],
				lang: ["afr", "deu", "eng", "her", "hgm", "kwn", "loz", "ndo", "tsn"]
			},
			nc: {
				name: "New Caledonia",
				official: "New Caledonia",
				native: ["Nouvelle-Calédonie"],
				alt: ["NC", "NCL"],
				callingCode: ["687"],
				currency: ["XPF"],
				lang: ["fra"]
			},
			ne: {
				name: "Niger",
				official: "Republic of Niger",
				native: ["Niger"],
				alt: ["NE", "Nijar", "NER"],
				callingCode: ["227"],
				currency: ["XOF"],
				lang: ["fra"]
			},
			nf: {
				name: "Norfolk Island",
				official: "Territory of Norfolk Island",
				native: ["Norfolk Island", "Norf'k Ailen"],
				alt: ["NF", "Teratri of Norf'k Ailen", "NFK"],
				callingCode: ["672"],
				currency: ["AUD"],
				lang: ["eng", "pih"]
			},
			ng: {
				name: "Nigeria",
				official: "Federal Republic of Nigeria",
				native: ["Nigeria"],
				alt: ["NG", "Nijeriya", "Naíjíríà", "NGA"],
				callingCode: ["234"],
				currency: ["NGN"],
				lang: ["eng"],
				timezone: ["UTC+01:00"]
			},
			ni: {
				name: "Nicaragua",
				official: "Republic of Nicaragua",
				native: ["Nicaragua"],
				alt: ["NI", "República de Nicaragua", "NIC"],
				callingCode: ["505"],
				currency: ["NIO"],
				lang: ["spa"],
				timezone: ["UTC-06:00"]
			},
			nl: {
				name: "Netherlands",
				official: "Kingdom of the Netherlands",
				native: ["Nederland"],
				alt: ["NL", "Holland", "Nederland", "The Netherlands", "NLD"],
				callingCode: ["31"],
				currency: ["EUR"],
				lang: ["nld"],
				timezone: ["UTC+01:00"]
			},
			no: {
				name: "Norway",
				official: "Kingdom of Norway",
				native: ["Noreg", "Norge", "Norgga"],
				alt: ["NO", "Norge", "Noreg", "Kongeriket Norge", "Kongeriket Noreg", "NOR"],
				callingCode: ["47"],
				currency: ["NOK"],
				lang: ["nno", "nob", "smi"],
				timezone: ["UTC+01:00"]
			},
			np: {
				name: "Nepal",
				official: "Federal Democratic Republic of Nepal",
				native: ["नेपाल"],
				alt: ["NP", "Loktāntrik Ganatantra Nepāl", "NPL"],
				callingCode: ["977"],
				currency: ["NPR"],
				lang: ["nep"],
				timezone: ["UTC+05:45"]
			},
			nr: {
				name: "Nauru",
				official: "Republic of Nauru",
				native: ["Nauru"],
				alt: ["NR", "Naoero", "Pleasant Island", "Ripublik Naoero", "NRU"],
				callingCode: ["674"],
				currency: ["AUD"],
				lang: ["eng", "nau"]
			},
			nu: {
				name: "Niue",
				official: "Niue",
				native: ["Niue", "Niuē"],
				alt: ["NU", "NIU"],
				callingCode: ["683"],
				currency: ["NZD"],
				lang: ["eng", "niu"]
			},
			nz: {
				name: "New Zealand",
				official: "New Zealand",
				native: ["New Zealand", "Aotearoa"],
				alt: ["NZ", "Aotearoa", "NZL"],
				callingCode: ["64"],
				currency: ["NZD"],
				lang: ["eng", "mri", "nzs"],
				timezone: ["UTC+12:00"]
			},
			om: {
				name: "Oman",
				official: "Sultanate of Oman",
				native: ["عمان"],
				alt: ["OM", "Salṭanat ʻUmān", "OMN"],
				callingCode: ["968"],
				currency: ["OMR"],
				lang: ["ara"],
				timezone: ["UTC+04:00"]
			},
			pa: {
				name: "Panama",
				official: "Republic of Panama",
				native: ["Panamá"],
				alt: ["PA", "República de Panamá", "PAN"],
				callingCode: ["507"],
				currency: ["PAB", "USD"],
				lang: ["spa"],
				timezone: ["UTC-05:00"]
			},
			pe: {
				name: "Peru",
				official: "Republic of Peru",
				native: ["Piruw", "Perú"],
				alt: ["PE", "República del Perú", "PER"],
				callingCode: ["51"],
				currency: ["PEN"],
				lang: ["aym", "que", "spa"],
				timezone: ["UTC-05:00"]
			},
			pf: {
				name: "French Polynesia",
				official: "French Polynesia",
				native: ["Polynésie française"],
				alt: ["PF", "Polynésie française", "Pōrīnetia Farāni", "PYF"],
				callingCode: ["689"],
				currency: ["XPF"],
				lang: ["fra"]
			},
			pg: {
				name: "Papua New Guinea",
				official: "Independent State of Papua New Guinea",
				native: ["Papua New Guinea", "Papua Niu Gini", "Papua Niugini"],
				alt: ["PG", "Independen Stet bilong Papua Niugini", "PNG"],
				callingCode: ["675"],
				currency: ["PGK"],
				lang: ["eng", "hmo", "tpi"]
			},
			ph: {
				name: "Philippines",
				official: "Republic of the Philippines",
				native: ["Philippines", "Pilipinas"],
				alt: ["PH", "Repúblika ng Pilipinas", "PHL"],
				callingCode: ["63"],
				currency: ["PHP"],
				lang: ["eng", "fil"],
				timezone: ["UTC+08:00"]
			},
			pk: {
				name: "Pakistan",
				official: "Islamic Republic of Pakistan",
				native: ["Pakistan", "پاكستان"],
				alt: ["PK", "Pākistān", "Islāmī Jumhūriya'eh Pākistān", "PAK"],
				callingCode: ["92"],
				currency: ["PKR"],
				lang: ["eng", "urd"],
				timezone: ["UTC+05:00"]
			},
			pl: {
				name: "Poland",
				official: "Republic of Poland",
				native: ["Polska"],
				alt: ["PL", "Rzeczpospolita Polska", "POL"],
				callingCode: ["48"],
				currency: ["PLN"],
				lang: ["pol"],
				timezone: ["UTC+01:00"]
			},
			pm: {
				name: "Saint Pierre and Miquelon",
				official: "Saint Pierre and Miquelon",
				native: ["Saint-Pierre-et-Miquelon"],
				alt: ["PM", "Collectivité territoriale de Saint-Pierre-et-Miquelon", "SPM"],
				callingCode: ["508"],
				currency: ["EUR"],
				lang: ["fra"]
			},
			pn: {
				name: "Pitcairn Islands",
				official: "Pitcairn Group of Islands",
				native: ["Pitcairn Islands"],
				alt: ["PN", "Pitcairn", "Pitcairn Henderson Ducie and Oeno Islands", "PCN"],
				callingCode: ["64"],
				currency: ["NZD"],
				lang: ["eng"]
			},
			pr: {
				name: "Puerto Rico",
				official: "Commonwealth of Puerto Rico",
				native: ["Puerto Rico"],
				alt: ["PR", "Estado Libre Asociado de Puerto Rico", "PRI"],
				callingCode: ["1787", "1939"],
				currency: ["USD"],
				lang: ["eng", "spa"],
				timezone: ["UTC-04:00"]
			},
			ps: {
				name: "Palestine",
				official: "State of Palestine",
				native: ["فلسطين"],
				alt: ["PS", "Palestine, State of", "Dawlat Filasṭin", "PSE"],
				callingCode: ["970"],
				currency: ["ILS"],
				lang: ["ara"]
			},
			pt: {
				name: "Portugal",
				official: "Portuguese Republic",
				native: ["Portugal"],
				alt: ["PT", "Portuguesa", "República Portuguesa", "PRT"],
				callingCode: ["351"],
				currency: ["EUR"],
				lang: ["por"],
				timezone: ["UTC+00:00", "UTC-01:00"]
			},
			pw: {
				name: "Palau",
				official: "Republic of Palau",
				native: ["Palau", "Belau"],
				alt: ["PW", "Beluu er a Belau", "PLW"],
				callingCode: ["680"],
				currency: ["USD"],
				lang: ["eng", "pau"]
			},
			py: {
				name: "Paraguay",
				official: "Republic of Paraguay",
				native: ["Paraguái", "Paraguay"],
				alt: ["PY", "República del Paraguay", "Tetã Paraguái", "PRY"],
				callingCode: ["595"],
				currency: ["PYG"],
				lang: ["grn", "spa"],
				timezone: ["UTC-04:00"]
			},
			qa: {
				name: "Qatar",
				official: "State of Qatar",
				native: ["قطر"],
				alt: ["QA", "Dawlat Qaṭar", "QAT"],
				callingCode: ["974"],
				currency: ["QAR"],
				lang: ["ara"],
				timezone: ["UTC+03:00"]
			},
			re: {
				name: "Réunion",
				official: "Réunion Island",
				native: ["La Réunion"],
				alt: ["RE", "Reunion", "REU"],
				callingCode: ["262"],
				currency: ["EUR"],
				lang: ["fra"],
				timezone: ["UTC+04:00"]
			},
			ro: {
				name: "Romania",
				official: "Romania",
				native: ["România"],
				alt: ["RO", "Rumania", "Roumania", "România", "ROU"],
				callingCode: ["40"],
				currency: ["RON"],
				lang: ["ron"],
				timezone: ["UTC+02:00"]
			},
			rs: {
				name: "Serbia",
				official: "Republic of Serbia",
				native: ["Србија"],
				alt: ["RS", "Srbija", "Република Србија", "Republika Srbija", "SRB"],
				callingCode: ["381"],
				currency: ["RSD"],
				lang: ["srp"],
				timezone: ["UTC+01:00"]
			},
			ru: {
				name: "Russia",
				official: "Russian Federation",
				native: ["Россия"],
				alt: ["RU", "Российская Федерация", "RUS"],
				callingCode: ["7"],
				currency: ["RUB"],
				lang: ["rus"],
				timezone: [
					"UTC+02:00",
					"UTC+03:00",
					"UTC+04:00",
					"UTC+05:00",
					"UTC+07:00",
					"UTC+08:00",
					"UTC+09:00",
					"UTC+10:00",
					"UTC+11:00",
					"UTC+12:00"
				]
			},
			rw: {
				name: "Rwanda",
				official: "Republic of Rwanda",
				native: ["Rwanda"],
				alt: ["RW", "Repubulika y'u Rwanda", "République du Rwanda", "RWA"],
				callingCode: ["250"],
				currency: ["RWF"],
				lang: ["eng", "fra", "kin"],
				timezone: ["UTC+02:00"]
			},
			sa: {
				name: "Saudi Arabia",
				official: "Kingdom of Saudi Arabia",
				native: ["العربية السعودية"],
				alt: ["Saudi", "SA", "Al-Mamlakah al-‘Arabiyyah as-Su‘ūdiyyah", "SAU"],
				callingCode: ["966"],
				currency: ["SAR"],
				lang: ["ara"],
				timezone: ["UTC+03:00"]
			},
			sb: {
				name: "Solomon Islands",
				official: "Solomon Islands",
				native: ["Solomon Islands"],
				alt: ["SB", "SLB"],
				callingCode: ["677"],
				currency: ["SBD"],
				lang: ["eng"]
			},
			sc: {
				name: "Seychelles",
				official: "Republic of Seychelles",
				native: ["Sesel", "Seychelles"],
				alt: ["SC", "Repiblik Sesel", "République des Seychelles", "SYC"],
				callingCode: ["248"],
				currency: ["SCR"],
				lang: ["crs", "eng", "fra"]
			},
			sd: {
				name: "Sudan",
				official: "Republic of the Sudan",
				native: ["السودان", "Sudan"],
				alt: ["SD", "Jumhūrīyat as-Sūdān", "SDN"],
				callingCode: ["249"],
				currency: ["SDG"],
				lang: ["ara", "eng"]
			},
			se: {
				name: "Sweden",
				official: "Kingdom of Sweden",
				native: ["Sverige"],
				alt: ["SE", "Konungariket Sverige", "SWE"],
				callingCode: ["46"],
				currency: ["SEK"],
				lang: ["swe"],
				timezone: ["UTC+01:00"]
			},
			sg: {
				name: "Singapore",
				official: "Republic of Singapore",
				native: ["新加坡", "Singapore", "Singapura", "சிங்கப்பூர்"],
				alt: ["SG", "Singapura", "Republik Singapura", "新加坡共和国", "SGP"],
				callingCode: ["65"],
				currency: ["SGD"],
				lang: ["zho", "eng", "msa", "tam"],
				timezone: ["UTC+08:00"]
			},
			sh: {
				name: "Saint Helena, Ascension and Tristan da Cunha",
				official: "Saint Helena, Ascension and Tristan da Cunha",
				native: ["Saint Helena, Ascension and Tristan da Cunha"],
				alt: ["Saint Helena", "St. Helena, Ascension and Tristan da Cunha", "SHN"],
				callingCode: ["290", "247"],
				currency: ["SHP", "GBP"],
				lang: ["eng"]
			},
			si: {
				name: "Slovenia",
				official: "Republic of Slovenia",
				native: ["Slovenija"],
				alt: ["SI", "Republika Slovenija", "SVN"],
				callingCode: ["386"],
				currency: ["EUR"],
				lang: ["slv"],
				timezone: ["UTC+01:00"]
			},
			sj: {
				name: "Svalbard and Jan Mayen",
				official: "Svalbard og Jan Mayen",
				native: ["Svalbard og Jan Mayen"],
				alt: ["SJ", "Svalbard and Jan Mayen Islands", "SJM"],
				callingCode: ["4779"],
				currency: ["NOK"],
				lang: ["nor"]
			},
			sk: {
				name: "Slovakia",
				official: "Slovak Republic",
				native: ["Slovensko"],
				alt: ["SK", "Slovenská republika", "SVK"],
				callingCode: ["421"],
				currency: ["EUR"],
				lang: ["slk"],
				timezone: ["UTC+01:00"]
			},
			sl: {
				name: "Sierra Leone",
				official: "Republic of Sierra Leone",
				native: ["Sierra Leone"],
				alt: ["SL", "SLE"],
				callingCode: ["232"],
				currency: ["SLL"],
				lang: ["eng"]
			},
			sm: {
				name: "San Marino",
				official: "Most Serene Republic of San Marino",
				native: ["San Marino"],
				alt: ["SM", "Republic of San Marino", "Repubblica di San Marino", "SMR"],
				callingCode: ["378"],
				currency: ["EUR"],
				lang: ["ita"]
			},
			sn: {
				name: "Senegal",
				official: "Republic of Senegal",
				native: ["Sénégal"],
				alt: ["SN", "République du Sénégal", "SEN"],
				callingCode: ["221"],
				currency: ["XOF"],
				lang: ["fra"],
				timezone: ["UTC+00:00"]
			},
			so: {
				name: "Somalia",
				official: "Federal Republic of Somalia",
				native: ["الصومال‎‎", "Soomaaliya"],
				alt: ["SO", "aṣ-Ṣūmāl", "Jamhuuriyadda Federaalka Soomaaliya", "Jumhūriyyat aṣ-Ṣūmāl al-Fiderāliyya", "SOM"],
				callingCode: ["252"],
				currency: ["SOS"],
				lang: ["ara", "som"],
				timezone: ["UTC+03:00"]
			},
			sr: {
				name: "Suriname",
				official: "Republic of Suriname",
				native: ["Suriname"],
				alt: ["SR", "Sarnam", "Sranangron", "Republiek Suriname", "SUR"],
				callingCode: ["597"],
				currency: ["SRD"],
				lang: ["nld"]
			},
			ss: {
				name: "South Sudan",
				official: "Republic of South Sudan",
				native: ["South Sudan"],
				alt: ["SS", "SSD"],
				callingCode: ["211"],
				currency: ["SSP"],
				lang: ["eng"]
			},
			st: {
				name: "São Tomé and Príncipe",
				official: "Democratic Republic of São Tomé and Príncipe",
				native: ["São Tomé e Príncipe"],
				alt: ["ST", "Sao Tome and Principe", "República Democrática de São Tomé e Príncipe", "STP"],
				callingCode: ["239"],
				currency: ["STD"],
				lang: ["por"]
			},
			sv: {
				name: "El Salvador",
				official: "Republic of El Salvador",
				native: ["El Salvador"],
				alt: ["SV", "República de El Salvador", "SLV"],
				callingCode: ["503"],
				currency: ["SVC", "USD"],
				lang: ["spa"],
				timezone: ["UTC-06:00"]
			},
			sx: {
				name: "Sint Maarten",
				official: "Sint Maarten",
				native: ["Sint Maarten", "Saint-Martin"],
				alt: ["SX", "Sint Maarten (Dutch part)", "SXM"],
				callingCode: ["1721"],
				currency: ["ANG"],
				lang: ["eng", "fra", "nld"]
			},
			sy: {
				name: "Syria",
				official: "Syrian Arab Republic",
				native: ["سوريا"],
				alt: ["SY", "Al-Jumhūrīyah Al-ʻArabīyah As-Sūrīyah", "SYR"],
				callingCode: ["963"],
				currency: ["SYP"],
				lang: ["ara"],
				timezone: ["UTC+02:00"]
			},
			sz: {
				name: "Eswatini",
				official: "Kingdom of Eswatini",
				native: ["Eswatini", "eSwatini"],
				alt: ["SZ", "Swaziland", "weSwatini", "Swatini", "Ngwane", "Umbuso weSwatini", "SWZ"],
				callingCode: ["268"],
				currency: ["SZL"],
				lang: ["eng", "ssw"]
			},
			tc: {
				name: "Turks and Caicos Islands",
				official: "Turks and Caicos Islands",
				native: ["Turks and Caicos Islands"],
				alt: ["TC", "TCA"],
				callingCode: ["1649"],
				currency: ["USD"],
				lang: ["eng"]
			},
			td: {
				name: "Chad",
				official: "Republic of Chad",
				native: ["تشاد‎", "Tchad"],
				alt: ["TD", "Tchad", "République du Tchad", "TCD"],
				callingCode: ["235"],
				currency: ["XAF"],
				lang: ["ara", "fra"]
			},
			tf: {
				name: "French Southern and Antarctic Lands",
				official: "Territory of the French Southern and Antarctic Lands",
				native: ["Terres australes et antarctiques françaises"],
				alt: ["TF", "French Southern Territories", "ATF"],
				callingCode: [],
				currency: ["EUR"],
				lang: ["fra"]
			},
			tg: {
				name: "Togo",
				official: "Togolese Republic",
				native: ["Togo"],
				alt: ["TG", "Togolese", "République Togolaise", "TGO"],
				callingCode: ["228"],
				currency: ["XOF"],
				lang: ["fra"]
			},
			th: {
				name: "Thailand",
				official: "Kingdom of Thailand",
				native: ["ประเทศไทย"],
				alt: ["TH", "Prathet", "Thai", "ราชอาณาจักรไทย", "Ratcha Anachak Thai", "THA"],
				callingCode: ["66"],
				currency: ["THB"],
				lang: ["tha"],
				timezone: ["UTC+07:00"]
			},
			tj: {
				name: "Tajikistan",
				official: "Republic of Tajikistan",
				native: ["Таджикистан", "Тоҷикистон"],
				alt: ["TJ", "Toçikiston", "Ҷумҳурии Тоҷикистон", "Çumhuriyi Toçikiston", "TJK"],
				callingCode: ["992"],
				currency: ["TJS"],
				lang: ["rus", "tgk"],
				timezone: ["UTC+05:00"]
			},
			tk: {
				name: "Tokelau",
				official: "Tokelau",
				native: ["Tokelau"],
				alt: ["TK", "TKL"],
				callingCode: ["690"],
				currency: ["NZD"],
				lang: ["eng", "smo", "tkl"]
			},
			tl: {
				name: "Timor-Leste",
				official: "Democratic Republic of Timor-Leste",
				native: ["Timor-Leste", "Timór-Leste"],
				alt: [
					"TL",
					"East Timor",
					"República Democrática de Timor-Leste",
					"Repúblika Demokrátika Timór-Leste",
					"Timór Lorosa'e",
					"Timor Lorosae",
					"TLS"
				],
				callingCode: ["670"],
				currency: ["USD"],
				lang: ["por", "tet"]
			},
			tm: {
				name: "Turkmenistan",
				official: "Turkmenistan",
				native: ["Туркмения", "Türkmenistan"],
				alt: ["TM", "TKM"],
				callingCode: ["993"],
				currency: ["TMT"],
				lang: ["rus", "tuk"],
				timezone: ["UTC+05:00"]
			},
			tn: {
				name: "Tunisia",
				official: "Tunisian Republic",
				native: ["تونس"],
				alt: ["TN", "Republic of Tunisia", "al-Jumhūriyyah at-Tūnisiyyah", "TUN"],
				callingCode: ["216"],
				currency: ["TND"],
				lang: ["ara"],
				timezone: ["UTC+01:00"]
			},
			to: {
				name: "Tonga",
				official: "Kingdom of Tonga",
				native: ["Tonga"],
				alt: ["TO", "TON"],
				callingCode: ["676"],
				currency: ["TOP"],
				lang: ["eng", "ton"]
			},
			tr: {
				name: "Turkey",
				official: "Republic of Turkey",
				native: ["Türkiye"],
				alt: ["TR", "Turkiye", "Türkiye Cumhuriyeti", "TUR"],
				callingCode: ["90"],
				currency: ["TRY"],
				lang: ["tur"],
				timezone: ["UTC+02:00"]
			},
			tt: {
				name: "Trinidad and Tobago",
				official: "Republic of Trinidad and Tobago",
				native: ["Trinidad and Tobago"],
				alt: ["TT", "TTO"],
				callingCode: ["1868"],
				currency: ["TTD"],
				lang: ["eng"],
				timezone: ["UTC-04:00"]
			},
			tv: {
				name: "Tuvalu",
				official: "Tuvalu",
				native: ["Tuvalu"],
				alt: ["TV", "TUV"],
				callingCode: ["688"],
				currency: ["AUD"],
				lang: ["eng", "tvl"]
			},
			tw: {
				name: "Taiwan",
				official: "Republic of China (Taiwan)",
				native: ["台灣"],
				alt: ["TW", "Táiwān", "Republic of China", "中華民國", "Zhōnghuá Mínguó", "Chinese Taipei", "TWN"],
				callingCode: ["886"],
				currency: ["TWD"],
				lang: ["zho"],
				timezone: ["UTC+08:00"]
			},
			tz: {
				name: "Tanzania",
				official: "United Republic of Tanzania",
				native: ["Tanzania"],
				alt: ["TZ", "Tanzania, United Republic of", "Jamhuri ya Muungano wa Tanzania", "TZA"],
				callingCode: ["255"],
				currency: ["TZS"],
				lang: ["eng", "swa"]
			},
			ua: {
				name: "Ukraine",
				official: "Ukraine",
				native: ["Україна"],
				alt: ["UA", "Ukrayina", "UKR"],
				callingCode: ["380"],
				currency: ["UAH"],
				lang: ["ukr"],
				timezone: ["UTC+02:00"]
			},
			ug: {
				name: "Uganda",
				official: "Republic of Uganda",
				native: ["Uganda"],
				alt: ["UG", "Jamhuri ya Uganda", "UGA"],
				callingCode: ["256"],
				currency: ["UGX"],
				lang: ["eng", "swa"]
			},
			um: {
				name: "United States Minor Outlying Islands",
				official: "United States Minor Outlying Islands",
				native: ["United States Minor Outlying Islands"],
				alt: ["UM", "UMI"],
				callingCode: [],
				currency: ["USD"],
				lang: ["eng"]
			},
			us: {
				name: "United States",
				official: "United States of America",
				native: ["United States"],
				alt: ["US", "USA", "USA"],
				callingCode: ["1"],
				currency: ["USD"],
				lang: ["eng"],
				timezone: ["UTC-05:00", "UTC-06:00", "UTC-07:00", "UTC-08:00", "UTC-09:00", "UTC-10:00"]
			},
			uy: {
				name: "Uruguay",
				official: "Oriental Republic of Uruguay",
				native: ["Uruguay"],
				alt: ["UY", "República Oriental del Uruguay", "URY"],
				callingCode: ["598"],
				currency: ["UYU"],
				lang: ["spa"],
				timezone: ["UTC-03:00"]
			},
			uz: {
				name: "Uzbekistan",
				official: "Republic of Uzbekistan",
				native: ["Узбекистан", "O‘zbekiston"],
				alt: ["UZ", "O‘zbekiston Respublikasi", "Ўзбекистон Республикаси", "UZB"],
				callingCode: ["998"],
				currency: ["UZS"],
				lang: ["rus", "uzb"],
				timezone: ["UTC+05:00"]
			},
			va: {
				name: "Vatican City",
				official: "Vatican City State",
				native: ["Vaticano", "Vaticanæ"],
				alt: ["VA", "Holy See (Vatican City State)", "Stato della Città del Vaticano", "VAT"],
				callingCode: ["3906698", "379"],
				currency: ["EUR"],
				lang: ["ita", "lat"]
			},
			vc: {
				name: "Saint Vincent and the Grenadines",
				official: "Saint Vincent and the Grenadines",
				native: ["Saint Vincent and the Grenadines"],
				alt: ["VC", "VCT"],
				callingCode: ["1784"],
				currency: ["XCD"],
				lang: ["eng"]
			},
			ve: {
				name: "Venezuela",
				official: "Bolivarian Republic of Venezuela",
				native: ["Venezuela"],
				alt: ["VE", "Venezuela, Bolivarian Republic of", "República Bolivariana de Venezuela", "VEN"],
				callingCode: ["58"],
				currency: ["VEF"],
				lang: ["spa"],
				timezone: ["UTC-04:00"]
			},
			vg: {
				name: "British Virgin Islands",
				official: "Virgin Islands",
				native: ["British Virgin Islands"],
				alt: ["VG", "Virgin Islands, British", "VGB"],
				callingCode: ["1284"],
				currency: ["USD"],
				lang: ["eng"]
			},
			vi: {
				name: "United States Virgin Islands",
				official: "Virgin Islands of the United States",
				native: ["United States Virgin Islands"],
				alt: ["VI", "Virgin Islands, U.S.", "VIR"],
				callingCode: ["1340"],
				currency: ["USD"],
				lang: ["eng"]
			},
			vn: {
				name: "Vietnam",
				official: "Socialist Republic of Vietnam",
				native: ["Việt Nam"],
				alt: ["VN", "Cộng hòa Xã hội chủ nghĩa Việt Nam", "Viet Nam", "VNM"],
				callingCode: ["84"],
				currency: ["VND"],
				lang: ["vie"],
				timezone: ["UTC+07:00"]
			},
			vu: {
				name: "Vanuatu",
				official: "Republic of Vanuatu",
				native: ["Vanuatu"],
				alt: ["VU", "Ripablik blong Vanuatu", "République de Vanuatu", "VUT"],
				callingCode: ["678"],
				currency: ["VUV"],
				lang: ["bis", "eng", "fra"]
			},
			wf: {
				name: "Wallis and Futuna",
				official: "Territory of the Wallis and Futuna Islands",
				native: ["Wallis et Futuna"],
				alt: ["WF", "Territoire des îles Wallis et Futuna", "WLF"],
				callingCode: ["681"],
				currency: ["XPF"],
				lang: ["fra"]
			},
			ws: {
				name: "Samoa",
				official: "Independent State of Samoa",
				native: ["Samoa", "Sāmoa"],
				alt: ["WS", "Malo Saʻoloto Tutoʻatasi o Sāmoa", "WSM"],
				callingCode: ["685"],
				currency: ["WST"],
				lang: ["eng", "smo"]
			},
			ye: {
				name: "Yemen",
				official: "Republic of Yemen",
				native: ["اليَمَن"],
				alt: ["YE", "Yemeni Republic", "al-Jumhūriyyah al-Yamaniyyah", "YEM"],
				callingCode: ["967"],
				currency: ["YER"],
				lang: ["ara"],
				timezone: ["UTC+03:00"]
			},
			yt: {
				name: "Mayotte",
				official: "Department of Mayotte",
				native: ["Mayotte"],
				alt: ["YT", "Département de Mayotte", "MYT"],
				callingCode: ["262"],
				currency: ["EUR"],
				lang: ["fra"]
			},
			za: {
				name: "South Africa",
				official: "Republic of South Africa",
				native: [
					"South Africa",
					"Sewula Afrika",
					"Afrika-Borwa",
					"Afrika Borwa",
					"Ningizimu Afrika",
					"Aforika Borwa",
					"Afrika Dzonga",
					"Afurika Tshipembe",
					"Mzantsi Afrika"
				],
				alt: ["ZA", "RSA", "Suid-Afrika", "ZAF"],
				callingCode: ["27"],
				currency: ["ZAR"],
				lang: ["afr", "eng", "nbl", "nso", "sot", "ssw", "tsn", "tso", "ven", "xho", "zul"],
				timezone: ["UTC+02:00"]
			},
			zm: {
				name: "Zambia",
				official: "Republic of Zambia",
				native: ["Zambia"],
				alt: ["ZM", "ZMB"],
				callingCode: ["260"],
				currency: ["ZMW"],
				lang: ["eng"]
			},
			zw: {
				name: "Zimbabwe",
				official: "Republic of Zimbabwe",
				native: ["Zimbabwe"],
				alt: ["ZW", "ZWE"],
				callingCode: ["263"],
				currency: ["ZWL"],
				lang: ["bwg", "eng", "kck", "khi", "ndc", "nde", "nya", "sna", "sot", "toi", "tsn", "tso", "ven", "xho", "zib"],
				timezone: ["UTC+02:00"]
			}
		});
	}

	async _createImage(width, height, bgColor) {
		return new Promise((resolve, reject) => {
			new Jimp(width, height, bgColor, (err, image) => {
				if (err) {
					reject(err);
				}
				else {
					resolve(image);
				}
			});
		});
	}

	async _svgToBuffer(path, width, height) {
		return new Promise((resolve, reject) => {
			Svg2img(path, { width: width, height: height }, (e, buffer) => {
				if (e) {
					return reject(e);
				}
				Jimp.read(buffer)
					.then(resolve)
					.catch(reject);
			});
		});
	}

	/**
	 * Generate the flag map and its metadata
	 */
	async generate(config) {
		config = Object.assign(
			{
				width: 24,
				height: 16,
				outputImage: "flags.output.png",
				outputJs: "flags.output.js",
				outputCss: "flags.output.css",
				maxFlagsPerWidth: 16
			},
			config
		);

		const countries = this.data;
		const nbCountries = Object.keys(countries).length;
		const flagsPath = Path.join(__dirname, "flags");
		const imageWidth = config.width * config.maxFlagsPerWidth;
		const imageHeight = config.height * Math.ceil(nbCountries / config.maxFlagsPerWidth);

		let image = await this._createImage(imageWidth, imageHeight, 0);

		console.log(
			"Creating flags image in " +
				config.width +
				"x" +
				config.height +
				" for " +
				Object.keys(countries) +
				" to " +
				config.outputImage +
				"..."
		);
		let index = 0;
		let countriesJs = {};
		let countriesCssList = [
			"*[class^=\"bzd-flag-\"] {display:inline-block;content:'';vertical-align:middle;background:url(" +
				Path.join(config.importOutputPath, Path.basename(config.outputImage)) +
				") no-repeat left top;width:" +
				config.width +
				"px;height:" +
				config.height +
				"px;}"
		];
		for (const code in countries) {
			try {
				const imageFlag = await this._svgToBuffer(Path.join(flagsPath, code + ".svg"), config.width, config.height);

				const offsetX = (index % config.maxFlagsPerWidth) * config.width;
				const offsetY = Math.floor(index / config.maxFlagsPerWidth) * config.height;

				countriesCssList.push(".bzd-flag-" + code + "{background-position: -" + offsetX + "px -" + offsetY + "px;}");

				image.composite(imageFlag, offsetX, offsetY);

				// Build the countries Javascript map

				// ---- Timezone ----
				let timezoneList = countries[code]["timezone"] || [];
				timezoneList = timezoneList
					.map((zone) => {
						if (zone == "UTC") {
							return 0;
						}
						const match = zone.match(/^UTC(.*)$/i);
						return match[1];
					})
					.filter((zone) => zone);
				if (!timezoneList.length) {
					console.error("Missing timezone for '" + code + "'");
				}

				// ---- Name ----
				const name = countries[code]["name"] || "";
				if (!name) {
					console.error("Missing name for '" + code + "'");
				}

				// ---- Search ----
				let searchList = countries[code]["official"] ? [countries[code]["official"]] : [];
				searchList = searchList.concat(countries[code]["native"] || []);
				searchList = searchList.concat(countries[code]["alt"] || []);
				searchList = searchList.concat(name ? [name] : []);
				searchList = searchList.map((item) => item.toLowerCase());
				searchList = [...new Set(searchList)];
				if (!searchList.length) {
					console.error("Missing search for '" + code + "'");
				}

				// ---- calling ----
				const callList = countries[code]["callingCode"] || [];
				if (!callList.length) {
					console.error("Missing call for '" + code + "'");
				}

				// ---- currency ----
				let currencyList = countries[code]["currency"] || [];
				currencyList = currencyList.map((currency) => currency.toLowerCase());
				if (!currencyList.length) {
					console.error("Missing currency for '" + code + "'");
				}

				// ---- lang ----
				let langList = countries[code]["lang"] || [];
				langList = langList.map((currency) => currency.toLowerCase());
				if (!langList.length) {
					console.error("Missing lang for '" + code + "'");
				}

				countriesJs[code] = {
					name: name,
					timezone: timezoneList,
					search: searchList,
					call: callList,
					currency: currencyList,
					lang: langList
				};
			}
			catch (e) {
				console.error(e);
			}

			// Increase the index
			index++;
		}

		// Create the image
		await image.write(config.outputImage);

		// Create the Javascript
		let dataJs = Flags.dataToJs(countriesJs);
		dataJs =
			"// Note: this file is automatically generated, all modifications will be lost.\n\"use strict\";\n\nexport default " +
			dataJs;
		await Fs.writeFileSync(config.outputJs, dataJs);

		// Create the Css
		await Fs.writeFileSync(config.outputCss, countriesCssList.join("\n"));
	}

	/**
	 * Nicely print the data to a Javascript format
	 */
	static dataToJs(data) {
		let lineList = [];
		for (const countryCode in data) {
			let line = "\t" + countryCode + ": {";
			let attrList = [];
			for (const key in data[countryCode]) {
				attrList.push(key + ": " + JSON.stringify(data[countryCode][key]));
			}
			line += attrList.join(", ");
			line += "}";
			lineList.push(line);
		}
		return "{\n" + lineList.join(",\n") + "\n}\n";
	}
}

const ouptutPath = Path.join(process.env["BUILD_WORKSPACE_DIRECTORY"] || __dirname, "nodejs/international");

(async () => {
	const flags = new Flags();
	const sizeList = [
		{ width: 24, height: 16 },
		{ width: 32, height: 24 },
		{ width: 48, height: 32 }
	];
	for (const i in sizeList) {
		console.log("Generating flags for " + sizeList[i].width + "x" + sizeList[i].height);
		await flags.generate({
			width: sizeList[i].width,
			height: sizeList[i].height,
			importOutputPath: "~bzd/international",
			outputJs: Path.join(ouptutPath, "countries.mjs"),
			outputImage: Path.join(ouptutPath, "flags_" + sizeList[i].width + "x" + sizeList[i].height + ".png"),
			outputCss: Path.join(ouptutPath, "flags_" + sizeList[i].width + "x" + sizeList[i].height + ".scss")
		});
	}
})();
