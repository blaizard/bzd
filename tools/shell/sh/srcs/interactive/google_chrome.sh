bzd_google_chrome() {
    # Start chrome in an isolated sandbox and it will not affect the main chrome profile.
    google-chrome --disable-site-isolation-trials --disable-web-security --allow-file-access-from-files --user-data-dir=/tmp
}
