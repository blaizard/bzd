use proc_macro::TokenStream;

use quote::{format_ident, quote};
use syn::parse::{Parse, ParseStream};
use syn::{Expr, LitStr, Token};

/// One literal piece of the format string, or an argument placeholder.
enum FormatPiece {
    Literal(String),
    Argument,
}

/// Parse the format literal, splitting it into literal and argument pieces.
///
/// Only `{}` placeholders are supported, together with `{{` and `}}` for
/// literal braces. Any other brace syntax is rejected.
fn parse_format(format: &LitStr) -> syn::Result<Vec<FormatPiece>> {
    let mut pieces = Vec::new();
    let mut literal = String::new();
    let value = format.value();
    let mut chars = value.chars().peekable();
    while let Some(c) = chars.next() {
        match c {
            '{' => {
                if chars.peek() == Some(&'{') {
                    chars.next();
                    literal.push('{');
                } else if chars.peek() == Some(&'}') {
                    chars.next();
                    if !literal.is_empty() {
                        pieces.push(FormatPiece::Literal(std::mem::take(&mut literal)));
                    }
                    pieces.push(FormatPiece::Argument);
                } else {
                    return Err(syn::Error::new(
                        format.span(),
                        "Unsupported format specifier, only '{}' is supported.",
                    ));
                }
            }
            '}' => {
                if chars.peek() == Some(&'}') {
                    chars.next();
                    literal.push('}');
                } else {
                    return Err(syn::Error::new(
                        format.span(),
                        "Unmatched '}' in format string.",
                    ));
                }
            }
            _ => literal.push(c),
        }
    }
    if !literal.is_empty() {
        pieces.push(FormatPiece::Literal(literal));
    }
    Ok(pieces)
}

struct LogInput {
    logger: Expr,
    format: LitStr,
    args: Vec<Expr>,
}

impl Parse for LogInput {
    fn parse(input: ParseStream) -> syn::Result<Self> {
        let logger: Expr = input.parse()?;
        input.parse::<Token![,]>()?;
        let format: LitStr = input.parse()?;
        let mut args = Vec::new();
        while input.parse::<Token![,]>().is_ok() {
            if input.is_empty() {
                break;
            }
            args.push(input.parse()?);
        }
        Ok(Self {
            logger,
            format,
            args,
        })
    }
}

/// Expand a log macro invocation into an async block that sends a lazy message
/// renderer and its source metadata to the logger's backend.
fn expand(input: TokenStream, level: &str) -> TokenStream {
    let input = syn::parse_macro_input!(input as LogInput);
    let logger = &input.logger;

    let pieces = match parse_format(&input.format) {
        Ok(pieces) => pieces,
        Err(error) => return error.to_compile_error().into(),
    };

    let arg_count = pieces
        .iter()
        .filter(|piece| matches!(piece, FormatPiece::Argument))
        .count();
    if arg_count != input.args.len() {
        let message = format!(
            "Incorrect number of arguments: expected {}, got {}.",
            arg_count,
            input.args.len()
        );
        return syn::Error::new(input.format.span(), message)
            .to_compile_error()
            .into();
    }

    let mut statements = quote! {};
    let mut arg_index: usize = 0;
    for piece in &pieces {
        match piece {
            FormatPiece::Literal(text) => {
                statements.extend(quote! { out.write(#text.as_bytes()).await?; });
            }
            FormatPiece::Argument => {
                let field = format_ident!("arg{}", arg_index);
                arg_index += 1;
                statements.extend(quote! { self.#field.write(out).await?; });
            }
        }
    }

    let argument_types: Vec<_> = (0..input.args.len())
        .map(|index| format_ident!("T{}", index))
        .collect();
    let argument_fields: Vec<_> = (0..input.args.len())
        .map(|index| format_ident!("arg{}", index))
        .collect();
    let struct_generics = if argument_types.is_empty() {
        quote! {}
    } else {
        quote! { <'a, #(#argument_types: ?Sized), *>}
    };
    let impl_generics = if argument_types.is_empty() {
        quote! {}
    } else {
        quote! { <'a, #(#argument_types: logger::LogValue + ?Sized), *>}
    };
    let type_generics = if argument_types.is_empty() {
        quote! {}
    } else {
        quote! { <'a, #(#argument_types), *> }
    };
    let struct_fields = argument_fields
        .iter()
        .zip(&argument_types)
        .map(|(field, ty)| {
            quote! { #field: &'a #ty }
        });
    let message_fields = argument_fields.iter().zip(&input.args).map(|(field, arg)| {
        quote! { #field: &(#arg) }
    });

    let level: syn::Path = syn::parse_str(level).expect("valid level path");

    quote! {
        async {
            let logger = &mut #logger;
            struct __BzdLogMessage #struct_generics {
                #(#struct_fields,)*
            }

            impl #impl_generics rust_interfaces_logger::LogMessage for __BzdLogMessage #type_generics {
                async fn write<__BzdOutput: logger::BzdOStreamInterface>(&self, out: &mut __BzdOutput) -> ::core::result::Result<(), ::bzd::base::error::Error> {
                    use logger::{BzdOStreamInterface as _, LogValue as _};
                    #statements
                    Ok(())
                }
            }

            let message = __BzdLogMessage {
                #(#message_fields,)*
            };
            let location = ::core::panic::Location::caller();
            logger.log(rust_interfaces_logger::LogRecord {
                level: #level,
                file: location.file(),
                line: location.line(),
                message,
            }).await?;
            Ok(())
        }
    }
    .into()
}

/// Log an error message.
#[proc_macro]
pub fn log_error(input: TokenStream) -> TokenStream {
    expand(input, "rust_interfaces_logger::BzdLoggerLevel::Error")
}

/// Log a warning message.
#[proc_macro]
pub fn log_warning(input: TokenStream) -> TokenStream {
    expand(input, "rust_interfaces_logger::BzdLoggerLevel::Warning")
}

/// Log an informational message.
#[proc_macro]
pub fn log_info(input: TokenStream) -> TokenStream {
    expand(input, "rust_interfaces_logger::BzdLoggerLevel::Info")
}

/// Log a debug message.
#[proc_macro]
pub fn log_debug(input: TokenStream) -> TokenStream {
    expand(input, "rust_interfaces_logger::BzdLoggerLevel::Debug")
}
