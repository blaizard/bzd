# Accounts

Web application to handle user accounts, authentication, permissions, information and subscriptions.

## Mission

This web application deals with the following:

- Authentication

## Test

```bash
bazel run //apps/accounts/backend --config=dev -- --test
```

Run the client application example.

```bash
bazel run //apps/accounts/example/static --config=dev -- --port 8081
```

Note by using `127.0.0.1` instead of `localhost`, the cookies between this example and the accounts app are not shared, this is a trick that can be used for testing locally.
