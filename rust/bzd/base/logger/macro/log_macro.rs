use proc_macro::TokenStream;

use quote::quote;
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

/// Expand a log macro invocation, writing each piece directly to the logger's
/// stream without any intermediate buffer, and propagating write errors to
/// the caller.
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
    let mut arg_index = 0;
    for piece in &pieces {
        match piece {
            FormatPiece::Literal(text) => {
                statements.extend(quote! { logger.write_str(#text).await?; });
            }
            FormatPiece::Argument => {
                let arg = &input.args[arg_index];
                arg_index += 1;
                statements.extend(quote! { logger.write_value(&#arg).await?; });
            }
        }
    }

    let level: syn::Path = syn::parse_str(level).expect("valid level path");

    quote! {
        {
            let logger = &mut #logger;
            if logger.is_enabled(#level) {
                logger.write_header(#level, ::core::panic::Location::caller()).await?;
                #statements
                logger.write_str("\n").await?;
            }
        }
    }
    .into()
}

/// Log an error message.
#[proc_macro]
pub fn log_error(input: TokenStream) -> TokenStream {
    expand(input, "::bzd::base::logger::Level::Error")
}

/// Log a warning message.
#[proc_macro]
pub fn log_warning(input: TokenStream) -> TokenStream {
    expand(input, "::bzd::base::logger::Level::Warning")
}

/// Log an informational message.
#[proc_macro]
pub fn log_info(input: TokenStream) -> TokenStream {
    expand(input, "::bzd::base::logger::Level::Info")
}

/// Log a debug message.
#[proc_macro]
pub fn log_debug(input: TokenStream) -> TokenStream {
    expand(input, "::bzd::base::logger::Level::Debug")
}
