# Applications

This repository contains public and private applications.

## Private

A private application is using git submodules pointing to a private subrepository.
This allow public users to share and use the repository without accessing private applications.

Private applications can be added as follow. Assuming we create a new application called `dummy`.
```bash
git submodule add -b main https://git.myprivaterepo.com/dummy.git apps/dummy
```
