import Template from "#bzd/nodejs/core/template.mjs";
import TemplateResetPasswordHtml from "#bzd/apps/accounts/backend/email/reset_password/reset_password_html.mjs";
import TemplateResetPasswordTxt from "#bzd/apps/accounts/backend/email/reset_password/reset_password_txt.mjs";

const html = new Template(TemplateResetPasswordHtml);
const text = new Template(TemplateResetPasswordTxt);

export default function (attributes) {
	return {
		html: html.process(attributes),
		text: text.process(attributes),
	};
}
