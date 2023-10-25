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
bazel run //apps/accounts/example --config=dev -- --accounts "http://localhost:8080"
```
