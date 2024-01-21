import Template from "#bzd/nodejs/core/template.mjs";
import TemplateHtml from "#bzd/apps/accounts/backend/email/welcome/welcome_html.mjs";
import TemplateTxt from "#bzd/apps/accounts/backend/email/welcome/welcome_txt.mjs";

const html = new Template(TemplateHtml);
const text = new Template(TemplateTxt);

export default function (attributes) {
	return {
		html: html.process(attributes),
		text: text.process(attributes),
	};
}
