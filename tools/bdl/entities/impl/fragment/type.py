import typing

from bzd.utils.memoized_property import memoized_property
from bzd.parser.element import Element, Sequence
from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.contract import Contracts


class Type:

	def __init__(self, element: Element, kind: str, template: typing.Optional[str] = None) -> None:

		Error.assertHasAttr(element=element, attr=kind)
		self.element = element
		self.kindAttr = kind
		self.templateAttr = template

	def resolve(self,
		symbols: typing.Any,
		namespace: typing.List[str],
		exclude: typing.Optional[typing.List[str]] = None) -> None:
		"""
		Resolve the types and nested templates by updating their symbol to fqn.
		"""

		fqn = symbols.resolveFQN(name=self.kind, namespace=namespace, exclude=exclude)
		Error.assertTrue(element=self.element,
			condition=(fqn is not None),
			message="Symbol '{}' in namespace '{}' could not be resolved.".format(self.kind, ".".join(namespace)))
		self.element.updateAttrValue(name=self.kindAttr, value=fqn)

		# Get the validation schema if any
		entity = symbols.getEntity(fqn=fqn)
		# TODO Implement some kind of validation
		if self.element.isNestedSequence("argument"):
			arguments = {(arg.getAttr("key").value if arg.isAttr("key") else str(i)): arg.getAttr("value").value
				for i, arg in enumerate(self.element.getNestedSequence("argument"))}  # type: ignore
			validation = entity.validation
			if validation is not None:
				print(validation)
				print(arguments)
				result = validation.validate(arguments, output="return")
				Error.assertTrue(element=self.element, condition=result, message=str(result))

		#entity.validate(self.kind)

		# Loop through the nested templates.
		if self.templateAttr:
			nested = self.element.getNestedSequence(self.templateAttr)
			if nested:
				for element in nested:
					# Recursively resolve nested types.
					# Note the "kind" here is always type for sub-elements.
					subType = Type(element=element, kind="type", template=self.templateAttr)
					subType.resolve(symbols=symbols, namespace=namespace)

	@property
	def kind(self) -> str:
		return self.element.getAttr(self.kindAttr).value

	@memoized_property
	def name(self) -> str:
		return Visitor(entity=self).result

	@property
	def isFQN(self) -> bool:
		"""
		Return True if this contains a fully qualified name already, False otherwise.
		"""
		return "." in self.kind

	def __repr__(self) -> str:
		return self.name  # type: ignore


class Visitor(VisitorBase[str, str]):

	nestedKind = "template"

	def __init__(self, entity: Type) -> None:

		# Deal with the main type, do not include the comment as
		# it is taken care by the upper level block.
		kind = self.visitType(kind=entity.kind, comment=None)

		# Construct the template if any.
		if entity.templateAttr is not None:
			self.nestedKind = entity.templateAttr
			nested = entity.element.getNestedSequence(self.nestedKind)
			template = None if nested is None else self._visit(sequence=nested)
		else:
			template = None

		self.result = self.visitTypeTemplate(kind=kind, template=template)

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitEnd(self, result: typing.List[str]) -> str:
		return self.visitTemplateItems(items=result)

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		Error.assertHasAttr(element=element, attr="type")
		result.append(self.visitType(kind=element.getAttr("type").value, comment=element.getAttrValue("comment")))
		return result

	def visitNested(self, element: Element, nestedSequence: Sequence, result: typing.List[str]) -> typing.List[str]:
		nestedResult = self._visit(nestedSequence)
		result[-1] = self.visitTypeTemplate(kind=result[-1], template=nestedResult)
		return result

	def visitType(self, kind: str, comment: typing.Optional[str]) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		return kind

	def visitTemplateItems(self, items: typing.List[str]) -> str:
		"""
		Called once all template elements have been discovered.
		This function should assemble the elements together.
		"""

		return "<{}>".format(",".join(items))

	def visitTypeTemplate(self, kind: str, template: typing.Optional[str]) -> str:
		"""
		Called to assemble a type with its template.
		"""

		if template is None:
			return kind
		return "{}{}".format(kind, template)
