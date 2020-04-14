<!-- Generated with Stardoc: http://skydoc.bazel.build -->

<a name="#bzd_nodejs_install"></a>

## bzd_nodejs_install

<pre>
bzd_nodejs_install(<a href="#bzd_nodejs_install-name">name</a>, <a href="#bzd_nodejs_install-alias">alias</a>, <a href="#bzd_nodejs_install-deps">deps</a>, <a href="#bzd_nodejs_install-packages">packages</a>, <a href="#bzd_nodejs_install-srcs">srcs</a>)
</pre>



**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :-------------: | :-------------: | :-------------: | :-------------: | :-------------: |
| name |  A unique name for this target.   | <a href="https://bazel.build/docs/build-ref.html#name">Name</a> | required |  |
| alias |  Name of the alias, available in the form [name], for the current directory.   | String | optional | "" |
| deps |  Dependencies   | <a href="https://bazel.build/docs/build-ref.html#labels">List of labels</a> | optional | [] |
| packages |  Package dependencies   | <a href="https://bazel.build/docs/skylark/lib/dict.html">Dictionary: String -> String</a> | optional | {} |
| srcs |  Source files   | <a href="https://bazel.build/docs/build-ref.html#labels">List of labels</a> | optional | [] |


<a name="#bzd_nodejs_library"></a>

## bzd_nodejs_library

<pre>
bzd_nodejs_library(<a href="#bzd_nodejs_library-name">name</a>, <a href="#bzd_nodejs_library-alias">alias</a>, <a href="#bzd_nodejs_library-deps">deps</a>, <a href="#bzd_nodejs_library-packages">packages</a>, <a href="#bzd_nodejs_library-srcs">srcs</a>)
</pre>



**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :-------------: | :-------------: | :-------------: | :-------------: | :-------------: |
| name |  A unique name for this target.   | <a href="https://bazel.build/docs/build-ref.html#name">Name</a> | required |  |
| alias |  Name of the alias, available in the form [name], for the current directory.   | String | optional | "" |
| deps |  Dependencies   | <a href="https://bazel.build/docs/build-ref.html#labels">List of labels</a> | optional | [] |
| packages |  Package dependencies   | <a href="https://bazel.build/docs/skylark/lib/dict.html">Dictionary: String -> String</a> | optional | {} |
| srcs |  Source files   | <a href="https://bazel.build/docs/build-ref.html#labels">List of labels</a> | optional | [] |


<a name="#BzdNodeJsDepsProvider"></a>

## BzdNodeJsDepsProvider

<pre>
BzdNodeJsDepsProvider(<a href="#BzdNodeJsDepsProvider-packages">packages</a>, <a href="#BzdNodeJsDepsProvider-srcs">srcs</a>, <a href="#BzdNodeJsDepsProvider-aliases">aliases</a>)
</pre>



**FIELDS**


| Name  | Description |
| :-------------: | :-------------: |
| packages |  (Undocumented)    |
| srcs |  (Undocumented)    |
| aliases |  (Undocumented)    |


<a name="#BzdNodeJsInstallProvider"></a>

## BzdNodeJsInstallProvider

<pre>
BzdNodeJsInstallProvider(<a href="#BzdNodeJsInstallProvider-package_json">package_json</a>, <a href="#BzdNodeJsInstallProvider-node_modules">node_modules</a>, <a href="#BzdNodeJsInstallProvider-aliases">aliases</a>)
</pre>



**FIELDS**


| Name  | Description |
| :-------------: | :-------------: |
| package_json |  (Undocumented)    |
| node_modules |  (Undocumented)    |
| aliases |  (Undocumented)    |


<a name="#bzd_nodejs_binary"></a>

## bzd_nodejs_binary

<pre>
bzd_nodejs_binary(<a href="#bzd_nodejs_binary-name">name</a>, <a href="#bzd_nodejs_binary-main">main</a>, <a href="#bzd_nodejs_binary-visibility">visibility</a>, <a href="#bzd_nodejs_binary-tags">tags</a>, <a href="#bzd_nodejs_binary-kwargs">kwargs</a>)
</pre>



**PARAMETERS**


| Name  | Description | Default Value |
| :-------------: | :-------------: | :-------------: |
| name |  <p align="center"> - </p>   |  none |
| main |  <p align="center"> - </p>   |  none |
| visibility |  <p align="center"> - </p>   |  <code>[]</code> |
| tags |  <p align="center"> - </p>   |  <code>[]</code> |
| kwargs |  <p align="center"> - </p>   |  none |


<a name="#bzd_nodejs_test"></a>

## bzd_nodejs_test

<pre>
bzd_nodejs_test(<a href="#bzd_nodejs_test-name">name</a>, <a href="#bzd_nodejs_test-main">main</a>, <a href="#bzd_nodejs_test-deps">deps</a>, <a href="#bzd_nodejs_test-visibility">visibility</a>, <a href="#bzd_nodejs_test-tags">tags</a>, <a href="#bzd_nodejs_test-kwargs">kwargs</a>)
</pre>



**PARAMETERS**


| Name  | Description | Default Value |
| :-------------: | :-------------: | :-------------: |
| name |  <p align="center"> - </p>   |  none |
| main |  <p align="center"> - </p>   |  none |
| deps |  <p align="center"> - </p>   |  <code>[]</code> |
| visibility |  <p align="center"> - </p>   |  <code>[]</code> |
| tags |  <p align="center"> - </p>   |  <code>[]</code> |
| kwargs |  <p align="center"> - </p>   |  none |


<a name="#bzd_nodejs_deps_aspect"></a>

## bzd_nodejs_deps_aspect

<pre>
bzd_nodejs_deps_aspect(<a href="#bzd_nodejs_deps_aspect-name">name</a>)
</pre>



**ASPECT ATTRIBUTES**


| Name | Type |
| :-------------: | :-------------: |
| deps| String |


**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :-------------: | :-------------: | :-------------: | :-------------: | :-------------: |
| name |  A unique name for this target.   | <a href="https://bazel.build/docs/build-ref.html#name">Name</a> | required |   |


