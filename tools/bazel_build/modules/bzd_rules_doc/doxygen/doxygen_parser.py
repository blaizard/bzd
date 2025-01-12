import argparse
import pathlib
import typing
import json
from xml.etree.ElementTree import Element, parse as xmlparse

Namespace = typing.List[str]
Json = typing.Dict[str, typing.Any]


class Data:

	def __init__(self) -> None:
		self.data: Json = {}

	@staticmethod
	def nameToNamespace(name: str) -> Namespace:
		return name.split("::")

	def add(self, name: str, data: typing.Dict[str, typing.Any]) -> typing.Dict[str, typing.Any]:
		namespace = Data.nameToNamespace(name)
		current = self.data
		for name in namespace:
			current = current.setdefault(name, {})
		current = current.setdefault("_", {})
		current.update(data)
		return current

	def toJson(self) -> str:
		return json.dumps(self.data, sort_keys=True, indent=4)


class Visitor:

	def __init__(self, index: pathlib.Path) -> None:
		self.index = index
		self.root = self.index.parent

	@staticmethod
	def parseXML(path: pathlib.Path) -> Element:
		return xmlparse(path).getroot()

	@staticmethod
	def escapeHTML(content: str) -> str:
		return content.replace("<", "&lt;").replace(">", "&gt;").replace("&", "&amp;")

	def visitParam(self, element: Element, data: Data) -> Json:
		visitors = {
		    "type": lambda child, attr: {
		        "type": child.text
		    },
		    "defname": lambda child, attr: {
		        "name": child.text
		    },
		    "defval": lambda child, attr: {
		        "default": child.text
		    },
		}
		param: typing.Dict[str, str] = {}
		for child in element:
			if child.tag in visitors:
				param.update(visitors[child.tag](child, child.attrib))  #type: ignore
		return param

	def visitTemplateParamList(self, element: Element, data: Data) -> Json:
		params: typing.List[typing.Any] = []
		for child in element:
			if child.tag == "param":
				params.append(self.visitParam(child, data))
		return {"template": params} if params else {}

	def visitLocation(self, element: Element, data: Data) -> Json:
		current = {}
		if "file" in element.attrib:
			current["path"] = element.attrib["file"]
		if "line" in element.attrib:
			current["line"] = element.attrib["line"]
		if "column" in element.attrib:
			current["column"] = element.attrib["column"]
		return {"location": current} if current else {}

	def visitDescription(self, element: Element, data: Data) -> str:

		tagMap = {
		    "bold": "b",
		    "emphasis": "em",
		    "computeroutput": "code",
		    "para": "p",
		    "programlisting": "pre",
		    "verbatim": "pre"
		}
		tagTextMap = {"codeline": "", "highlight": "", "sp": " "}
		content = ""
		for child in element:
			tag = tagMap.get(child.tag)
			if not tag:
				if child.tag in tagTextMap:
					content += tagTextMap[child.tag]
				else:
					print("IGNORED", child.tag)
					continue
			content += f"<{tag}>" if tag else ""
			content += "<code class=\"language-cpp\">" if tag == "pre" else ""
			if child.text:
				content += Visitor.escapeHTML(child.text)
			content += self.visitDescription(child, data)
			content += "</code>" if tag == "pre" else ""
			content += f"</{tag}>" if tag else ""
			if child.tail:
				content += child.tail

		return content

	def visitBriefDescription(self, element: Element, data: Data) -> Json:
		description = self.visitDescription(element, data)
		return {"doc": description} if description else {}

	def visitDetailedDescription(self, element: Element, data: Data) -> Json:
		description = self.visitDescription(element, data)
		return {"brief": description} if description else {}

	def visitCompounddef(self, element: Element, data: Data) -> None:

		kind = element.attrib["kind"]
		if kind not in (
		    "class",
		    "struct",
		):
			return

		visitors = {
		    "location": self.visitLocation,
		    "templateparamlist": self.visitTemplateParamList,
		    "briefdescription": self.visitBriefDescription,
		    "detaileddescription": self.visitDetailedDescription,
		}

		current = {"kind": kind}

		if element.attrib.get("final") == "yes":
			current["final"] = True

		for child in element:
			if child.tag == "compoundname":
				assert child.text
				current = data.add(child.text, current)
			elif child.tag in visitors:
				current.update(visitors[child.tag](child, data))

	def visitTopLevel(self, element: Element, data: Data) -> None:

		if element.tag == "compound":
			fileName = pathlib.Path(element.attrib["refid"] + ".xml")
			root = Visitor.parseXML(self.root / fileName)
			for child in root:
				self.visitTopLevel(child, data)

		if element.tag == "compounddef":
			self.visitCompounddef(element, data)

	def visit(self) -> Data:

		root = Visitor.parseXML(self.index)
		data = Data()

		for child in root:
			self.visitTopLevel(child, data)

		return data


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="XML doxygen parser.")
	parser.add_argument("--output",
	                    type=pathlib.Path,
	                    default="out.json",
	                    help="The output path of the resulting json file.")
	parser.add_argument(
	    "xml",
	    type=pathlib.Path,
	    help="Directory containing the XML files generated by doxygen.",
	)

	args = parser.parse_args()

	visitor = Visitor(index=args.xml / "index.xml")
	data = visitor.visit()

	print(args.xml)

	args.output.write_text(data.toJson())

	#index = args.xml / "index.xml"
	#print(xmlparse(index).getroot())
