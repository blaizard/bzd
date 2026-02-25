use proc_macro::TokenStream;
use quote::{format_ident, quote};
use syn::{parse_macro_input, Item, ItemMod};

// See: https://rustc-dev-guide.rust-lang.org/test-implementation.html

#[proc_macro_attribute]
pub fn test(_args: TokenStream, input: TokenStream) -> TokenStream {
    let mut item_mod = parse_macro_input!(input as ItemMod);
    let mut registrations = Vec::new();

    if let Some((_, content)) = &mut item_mod.content {
        for item in content.iter_mut() {
            if let Item::Fn(func) = item {
                let test_attr_index = func
                    .attrs
                    .iter()
                    .position(|attr| attr.path().is_ident("test"));
                if let Some(index) = test_attr_index {
                    // Remove the #[test] attribute to prevent the default test runner from picking it up.
                    func.attrs.remove(index);

                    let test_name = &func.sig.ident;
                    let reg_name = format_ident!("__REG_{}", test_name.to_string().to_uppercase());
                    //panic!("hello {}", test_name);

                    registrations.push(quote! {
                        #[used]
                        #[unsafe(link_section = "bzd_tests_data")]
                        static #reg_name: ::bzd::tests::TestMetadata = ::bzd::tests::TestMetadata {
                            name: stringify!(#test_name),
                            test_fn: #test_name,
                        };
                    });
                }
            }
        }

        for reg in registrations {
            content.push(syn::parse2(reg).unwrap());
        }
    }

    // Uncomment for debugging the generated code.
    //panic!("{}", quote!(#item_mod).to_string());
    quote!(#item_mod).into()
}
