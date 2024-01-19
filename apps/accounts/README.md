# Accounts

Web application to handle user accounts, authentication, permissions, information and subscriptions.

## Mission

This web application deals with the following:

- Authentication

## Test

```bash
bazel run //apps/accounts/backend --config=dev -- --test
```

### Test Stripe

```bash
stripe listen --forward-to http://localhost:8080/api/v1/stripe
stripe trigger payment_intent.succeeded
stripe trigger checkout.session.completed
```

### Serverless

This demonstrates server-less authentication.

Run the application example.

```bash
bazel run //apps/accounts/example/serverless --config=dev -- --port 8081
```

### Server

This demonstrates a typical frontend/backend application authentication.
Run the application example.

```bash
bazel run //apps/accounts/example/server --config=dev
```

## Push the docker image

```bash
bazel run apps/accounts:push --config=prod
```
