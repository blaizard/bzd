use proc_macro::TokenStream;
use quote::{format_ident, quote};
use syn::{parse_macro_input, Item, ItemMod};

// See: https://rustc-dev-guide.rust-lang.org/test-implementation.html

#[proc_macro_attribute]
pub fn test(_args: TokenStream, input: TokenStream) -> TokenStream {
    let mut item_mod = parse_macro_input!(input as ItemMod);
    let mut registrations = Vec::new();

    // Make sure the main module is linked.
    registrations.push(quote! {
        use main as _;
    });

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

                    // Check for #[ignore]
                    let is_ignored = func.attrs.iter().any(|attr| attr.path().is_ident("ignore"));

                    let test_name = &func.sig.ident;
                    let reg_name = format_ident!("__REG_{}", test_name.to_string().to_uppercase());

                    registrations.push(quote! {
                        #[used]
                        #[unsafe(link_section = "bzd_test_data")]
                        static #reg_name: ::bzd_test::tests::TestMetadata = ::bzd_test::tests::TestMetadata {
                            name: stringify!(#test_name),
                            test_fn: #test_name,
                            ignore: #is_ignored,
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
