
#include "compiler.h"
#include "diagnostics.h"
#include <string>
#include "parsers/ldml/ldml_parser.h"
#include "core/parsers/external_data.h"
#include <functional>
#include <libxml++/libxml++.h>

using namespace Lya::lib;
using namespace Lya::lib::utils;
using namespace Lya::lib::types;
using namespace Lya::core::parsers;
using namespace Lya::core::parsers::ldml;
using namespace Lya::core::parsers::message;
using namespace Lya::javascript_extension::diagnostics;

namespace Lya::javascript_extension {

	Compiler::Compiler(JavaScriptLanguage _javascript_language, int _supported_plural_categories, int _supported_ordinal_categories):
		javascript_language(_javascript_language),
		supported_plural_categories(_supported_plural_categories),
		supported_ordinal_categories(_supported_ordinal_categories),
		current_unique_identifier(0),
        scan_dependencies(false),
        should_write_plural_form_resolver(false),
        should_write_integer_digits_value_transform_function(false),
        should_write_number_of_fraction_digits_with_trailing_zero_value_transform_function(false),
        should_write_number_of_fraction_digits_without_trailing_zero_value_transform_function(false),
        should_write_visible_fractional_digits_with_trailing_zero_value_transform_function(false),
        should_write_visible_fractional_digits_without_trailing_zero_value_transform_function(false),
		w()
	{ }

	std::vector<Diagnostic> Compiler::compile(const std::vector<string>& localization_files, const std::string& language)
	{
		std::vector<Diagnostic> diagnostics;
		std::vector<LocalizationMessage> localizations;
		std::tie(localizations, diagnostics) = read_localization_files(localization_files);
		if (diagnostics.size() > 0) {
			return diagnostics;
		}
		message_parser = std::make_unique<MessageParser>(language);
		compile_messages(localizations);
        write_dependent_functions();
		w.print();
		return diagnostics;
	}

	void Compiler::compile_messages(const std::vector<LocalizationMessage>& localizations)
	{
		if (javascript_language == JavaScriptLanguage::TypeScript) {
			w.write_line("export namespace L {");
		}
		else {
			w.write_line("export const L = (function L() {");
		}
		w.indent();
		w.add_placeholder("after_namespace");
		for (const LocalizationMessage& l : localizations) {
			Messages messages = message_parser->parse(l.message);
			write_localization_functions(l.id, l.params, messages);
		}
		w.unindent();
        if (javascript_language == JavaScriptLanguage::TypeScript) {
            w.write_line("}");
        }
        else {
            w.write_line("})();");
        }
	}

    void Compiler::begin_observe_dependencies()
    {
        scan_dependencies = true;
        w.no_write = true;
    }

    void Compiler::end_observe_dependencies()
    {
        w.no_write = false;
        scan_dependencies = false;
    }
    void Compiler::write_dependent_functions()
    {
        w.begin_write_on_placeholder("after_namespace");

        begin_observe_dependencies();
        write_plural_form_resolver();
        end_observe_dependencies();

        std::vector<std::pair<bool Compiler::*, std::function<void()> > > dependent_functions = {
            { &Compiler::should_write_plural_form_resolver, std::bind(&Compiler::write_plural_form_resolver, this) },
            { &Compiler::should_write_integer_digits_value_transform_function, std::bind(&Compiler::write_integer_digits_value_transform_function, this) },
            { &Compiler::should_write_number_of_fraction_digits_with_trailing_zero_value_transform_function, std::bind(&Compiler::write_number_of_fraction_digits_with_trailing_zero_value_transform_function, this) },
            { &Compiler::should_write_number_of_fraction_digits_without_trailing_zero_value_transform_function, std::bind(&Compiler::write_number_of_fraction_digits_without_trailing_zero_value_transform_function, this) },
            { &Compiler::should_write_visible_fractional_digits_with_trailing_zero_value_transform_function, std::bind(&Compiler::write_visible_fractional_digits_with_trailing_zero_value_transform_function, this) },
            { &Compiler::should_write_visible_fractional_digits_without_trailing_zero_value_transform_function, std::bind(&Compiler::write_visible_fractional_digits_without_trailing_zero_value_transform_function, this) },
        };
        for (auto it = dependent_functions.rbegin(); it != dependent_functions.rend(); it++) {
            if (this->*(it->first)) {
                it->second();
            }
        }
        w.end_write_on_placeholder();
    }

	void Compiler::write_localization_functions(
		const std::string id,
		const std::vector<Parameter> params,
        const Messages& messages)
	{
		if (messages.size() == 0) {
			throw logic_error("Message size cannot be zero.");
		}
		if (javascript_language == JavaScriptLanguage::TypeScript) {
			w.write("export function " + id + "(");
		}
		else {
			w.write(id + ": function(");
		}
		for (const Parameter& p : params) {
			if (javascript_language == JavaScriptLanguage::TypeScript) {
				w.write(p.name + ": " + *p.type);
			}
			else {
				w.write(p.name);
			}
			w.save();
			w.write(", ");
		}
		w.restore();
		w.write(")");
		if (javascript_language == JavaScriptLanguage::TypeScript) {
			w.write(": void");
		}
		w.write_line(" {");
		w.indent();
//		if (messages.size() == 1 && ) {
//			w.write("return ");
//			messages[0]->accept(this);
//			w.write_line(";");
//		}
//		else {
			w.write_line("let s = '';");
			w.add_placeholder("after_variable_definition");
			for (const std::unique_ptr<Message>& message : messages) {
				message->accept(this);
			}
			w.write_line("return s;");
//		}
		w.unindent();
		w.write_line("},");
	}

	void Compiler::write_plural_form_resolver()
	{
		message_parser->read_plural_info();
		w.write_line("function gpf(n) {");
		w.indent();
		int i = 0;
		for (const PluralForm pf : *message_parser->supported_plural_forms_excluding_other) {
			std::unique_ptr<Expression>& ldml_expression = message_parser->plural_forms->at(pf);
			if (i == 0) {
				w.write("if (");
			}
			else {
				w.write("else if (");
			}
			ldml_expression->accept(this);
			w.write_line(") {");
			w.indent();
			w.write_line("return '" + plural_form_to_string.at(pf) + "';");
			w.unindent();
			w.write_line("}");
			i++;
		}
		w.write_line("return '" + plural_form_to_string.at(PluralForm::Other) + "';");
		w.unindent();
		w.write_line("}");
	}

	void Compiler::write_messages(const std::vector<std::unique_ptr<Message>>& messages)
	{
		for (const std::unique_ptr<Message>& m : messages) {
			m->accept(this);
		}
	}

	void Compiler::write_integer_digits_value_transform_function()
	{
		w.write_line("function i(n) {");
		w.indent();
		w.write_line("return Math.floor(n);");
		w.unindent();
		w.write_line("}");
	}

	void Compiler::write_number_of_fraction_digits_with_trailing_zero_value_transform_function()
	{
		w.write_line("function v(n) {");
		w.indent();
		w.write_line("const s = n.toString().split('.');");
		w.write_line("if (s.length > 0) {");
		w.indent();
		w.write_line("return s[1].length;");
		w.unindent();
		w.write_line("}");
		w.write_line("return 0;");
		w.unindent();
		w.write_line("}");
	}

	void Compiler::write_number_of_fraction_digits_without_trailing_zero_value_transform_function()
	{
		w.write_line("function w(n) {");
		w.indent();
		w.write_line("const s = n.toString().split('.');");
		w.write_line("if (s.length > 0) {");
		w.indent();
		w.write_line("let l = s[1].length;");
		w.write_line("for (var i = l - 1; i >= 0; i--) {");
		w.indent();
		w.write_line("if (s[1][i] === '0') {");
		w.indent();
		w.write_line("l--;");
		w.unindent();
		w.write_line("}");
		w.unindent();
		w.write_line("}");
		w.write_line("return l");
		w.unindent();
		w.write_line("}");
		w.write_line("return 0;");
		w.unindent();
		w.write_line("}");
	}

	void Compiler::write_visible_fractional_digits_with_trailing_zero_value_transform_function()
	{
		w.write_line("function f(n) {");
		w.indent();
		w.write_line("const s = n.toString().split('.');");
		w.write_line("if (s.length > 0) {");
		w.indent();
		w.write_line("return parseInt(s[1], 10);");
		w.unindent();
		w.write_line("}");
		w.write_line("return 0;");
		w.unindent();
		w.write_line("}");
	}

	void Compiler::write_visible_fractional_digits_without_trailing_zero_value_transform_function()
	{
		w.write_line("function t(n) {");
		w.indent();
		w.write_line("let s = n.toString().split('.');");
		w.write_line("if (s.length > 0) {");
		w.indent();
		w.write_line("for (var i = s[1].length - 1; i >= 0; i--) {");
		w.indent();
		w.write_line("if (s[1][i] === '0') {");
		w.indent();
		w.write_line("delete s[1][i];");
		w.unindent();
		w.write_line("}");
		w.unindent();
		w.write_line("}");
		w.write_line("return parseInt(s[1], 10);");
		w.unindent();
		w.write_line("}");
		w.write_line("return 0;");
		w.unindent();
		w.write_line("}");
	}

	/// LDML Nodes

	void Compiler::visit(const Expression* expression)
	{
		throw logic_error("should not reach here.");
	}

	void Compiler::visit(const TokenNode* token_node)
	{
		w.write(" " + ldml_token_enum_to_javascript_string.at(token_node->token) + " ");
	}

	void Compiler::visit(const IntegerLiteral* integer_literal)
	{
		w.write(std::to_string(integer_literal->value));
	}

	void Compiler::visit(const FloatLiteral* float_literal)
	{
		w.write(std::to_string(float_literal->value));
	}

	void Compiler::visit(const ValueTransform* value_transform)
	{
		switch (value_transform->type) {
			case ValueTransformType::AbsoluteValue:
                if (!scan_dependencies) {
                    w.write("n");
                }
				break;
			case ValueTransformType::IntegerDigitsValue:
                if (!scan_dependencies) {
				    w.write("i(n)");
                }
				should_write_integer_digits_value_transform_function = true;
				break;
			case ValueTransformType::NumberOfVisibleFractionDigits_WithTrailingZeros:
                if (!scan_dependencies) {
                    w.write("v(n)");
                }
				should_write_number_of_fraction_digits_with_trailing_zero_value_transform_function = true;
				break;
			case ValueTransformType::NumberOfVisibleFractionDigits_WithoutTrailingZeros:
                if (!scan_dependencies) {
                    w.write("w(n)");
                }
				should_write_number_of_fraction_digits_without_trailing_zero_value_transform_function = true;
				break;
			case ValueTransformType::VisibleFractionalDigits_WithTrailingZeros:
                if (!scan_dependencies) {
                    w.write("f(n)");
                }
				should_write_visible_fractional_digits_with_trailing_zero_value_transform_function = true;
				break;
			case ValueTransformType::VisibleFractionalDigits_WithoutTrailingZeros:
                if (!scan_dependencies) {
                    w.write("t(n)");
                }
				should_write_visible_fractional_digits_without_trailing_zero_value_transform_function = true;
				break;
		}
	}

	std::string Compiler::generate_unique_identifier()
	{
		return "_" + std::to_string(current_unique_identifier++);
	}

	void Compiler::visit(const BinaryExpression* binary_expression)
	{
		binary_expression->left_operand->accept(this);
		binary_expression->_operator->accept(this);
		binary_expression->right_operand->accept(this);
	}

	/// Message Nodes

	void Compiler::visit(const TextMessage* text_message)
	{
		w.write_line("s += \"" + text_message->text + "\";");
	}

	void Compiler::visit(const InterpolationMessage* interpolation_message)
	{
		w.write_line("s += " + interpolation_message->variable);
	}

	void Compiler::visit(const PluralMessage* plural_message)
	{
		const std::string id = generate_unique_identifier();
		w.write_line("switch ({0}) {", id);
		w.indent();
		const auto& vm = plural_message->value_messages;
		const auto& pm = plural_message->plural_form_messages;
		for (auto it = vm.begin(); it != vm.end(); it++) {
			w.write("case ");
			w.write(std::to_string(it->first));
			w.write_line(":");
			w.indent();
			write_messages(it->second);
			w.write_line("break;");
			w.unindent();
		}
		for (auto it = pm.begin(); it != pm.end(); it++) {
			if (it->first != PluralForm::Other) {
				w.write("case ");
				w.write("'" + plural_form_to_string.at(it->first) + "'");
				w.write_line(":");
			}
			else {
				w.write_line("default: ");
			}
			w.indent();
			write_messages(it->second);
			w.write_line("break;");
			w.unindent();
		}
		w.unindent();
		w.write_line("}"); // End of switch(p)

        w.begin_write_on_placeholder("after_variable_definition");
        w.write("const {0} = gpf(", id);
        w.write(plural_message->variable);
        w.write_line(");");
        w.end_write_on_placeholder();

        should_write_plural_form_resolver = true;
	}
}