<!-- Generated with Stardoc: http://skydoc.bazel.build -->



<a id="bzd_nodejs_install"></a>

## bzd_nodejs_install

<pre>
bzd_nodejs_install(<a href="#bzd_nodejs_install-name">name</a>, <a href="#bzd_nodejs_install-aliases">aliases</a>, <a href="#bzd_nodejs_install-data">data</a>, <a href="#bzd_nodejs_install-deps">deps</a>, <a href="#bzd_nodejs_install-packages">packages</a>, <a href="#bzd_nodejs_install-srcs">srcs</a>, <a href="#bzd_nodejs_install-tools">tools</a>)
</pre>



**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :------------- | :------------- | :------------- | :------------- | :------------- |
| <a id="bzd_nodejs_install-name"></a>name |  A unique name for this target.   | <a href="https://bazel.build/concepts/labels#target-names">Name</a> | required |  |
| <a id="bzd_nodejs_install-aliases"></a>aliases |  Name of the alias and path (relative to the workspace root).   | <a href="https://bazel.build/rules/lib/dict">Dictionary: String -> String</a> | optional | <code>{}</code> |
| <a id="bzd_nodejs_install-data"></a>data |  Data to be added to the runfile list   | <a href="https://bazel.build/concepts/labels">List of labels</a> | optional | <code>[]</code> |
| <a id="bzd_nodejs_install-deps"></a>deps |  Dependencies   | <a href="https://bazel.build/concepts/labels">List of labels</a> | optional | <code>[]</code> |
| <a id="bzd_nodejs_install-packages"></a>packages |  Package dependencies   | <a href="https://bazel.build/rules/lib/dict">Dictionary: String -> String</a> | optional | <code>{}</code> |
| <a id="bzd_nodejs_install-srcs"></a>srcs |  Source files   | <a href="https://bazel.build/concepts/labels">List of labels</a> | optional | <code>[]</code> |
| <a id="bzd_nodejs_install-tools"></a>tools |  Additional tools to be added to the runfile   | <a href="https://bazel.build/concepts/labels">List of labels</a> | optional | <code>[]</code> |


<a id="bzd_nodejs_library"></a>

## bzd_nodejs_library

<pre>
bzd_nodejs_library(<a href="#bzd_nodejs_library-name">name</a>, <a href="#bzd_nodejs_library-aliases">aliases</a>, <a href="#bzd_nodejs_library-data">data</a>, <a href="#bzd_nodejs_library-deps">deps</a>, <a href="#bzd_nodejs_library-packages">packages</a>, <a href="#bzd_nodejs_library-srcs">srcs</a>, <a href="#bzd_nodejs_library-tools">tools</a>)
</pre>



**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :------------- | :------------- | :------------- | :------------- | :------------- |
| <a id="bzd_nodejs_library-name"></a>name |  A unique name for this target.   | <a href="https://bazel.build/concepts/labels#target-names">Name</a> | required |  |
| <a id="bzd_nodejs_library-aliases"></a>aliases |  Name of the alias and path (relative to the workspace root).   | <a href="https://bazel.build/rules/lib/dict">Dictionary: String -> String</a> | optional | <code>{}</code> |
| <a id="bzd_nodejs_library-data"></a>data |  Data to be added to the runfile list   | <a href="https://bazel.build/concepts/labels">List of labels</a> | optional | <code>[]</code> |
| <a id="bzd_nodejs_library-deps"></a>deps |  Dependencies   | <a href="https://bazel.build/concepts/labels">List of labels</a> | optional | <code>[]</code> |
| <a id="bzd_nodejs_library-packages"></a>packages |  Package dependencies   | <a href="https://bazel.build/rules/lib/dict">Dictionary: String -> String</a> | optional | <code>{}</code> |
| <a id="bzd_nodejs_library-srcs"></a>srcs |  Source files   | <a href="https://bazel.build/concepts/labels">List of labels</a> | optional | <code>[]</code> |
| <a id="bzd_nodejs_library-tools"></a>tools |  Additional tools to be added to the runfile   | <a href="https://bazel.build/concepts/labels">List of labels</a> | optional | <code>[]</code> |


<a id="BzdNodeJsDepsProvider"></a>

## BzdNodeJsDepsProvider

<pre>
BzdNodeJsDepsProvider(<a href="#BzdNodeJsDepsProvider-packages">packages</a>, <a href="#BzdNodeJsDepsProvider-srcs">srcs</a>, <a href="#BzdNodeJsDepsProvider-aliases">aliases</a>, <a href="#BzdNodeJsDepsProvider-data">data</a>)
</pre>



**FIELDS**


| Name  | Description |
| :------------- | :------------- |
| <a id="BzdNodeJsDepsProvider-packages"></a>packages |  (Undocumented)    |
| <a id="BzdNodeJsDepsProvider-srcs"></a>srcs |  (Undocumented)    |
| <a id="BzdNodeJsDepsProvider-aliases"></a>aliases |  (Undocumented)    |
| <a id="BzdNodeJsDepsProvider-data"></a>data |  (Undocumented)    |


<a id="BzdNodeJsInstallProvider"></a>

## BzdNodeJsInstallProvider

<pre>
BzdNodeJsInstallProvider(<a href="#BzdNodeJsInstallProvider-package_json">package_json</a>, <a href="#BzdNodeJsInstallProvider-node_modules">node_modules</a>, <a href="#BzdNodeJsInstallProvider-aliases">aliases</a>)
</pre>



**FIELDS**


| Name  | Description |
| :------------- | :------------- |
| <a id="BzdNodeJsInstallProvider-package_json"></a>package_json |  (Undocumented)    |
| <a id="BzdNodeJsInstallProvider-node_modules"></a>node_modules |  (Undocumented)    |
| <a id="BzdNodeJsInstallProvider-aliases"></a>aliases |  (Undocumented)    |


<a id="bzd_nodejs_aliases_symlinks"></a>

## bzd_nodejs_aliases_symlinks

<pre>
bzd_nodejs_aliases_symlinks(<a href="#bzd_nodejs_aliases_symlinks-files">files</a>, <a href="#bzd_nodejs_aliases_symlinks-aliases">aliases</a>)
</pre>



**PARAMETERS**


| Name  | Description | Default Value |
| :------------- | :------------- | :------------- |
| <a id="bzd_nodejs_aliases_symlinks-files"></a>files |  <p align="center"> - </p>   |  none |
| <a id="bzd_nodejs_aliases_symlinks-aliases"></a>aliases |  <p align="center"> - </p>   |  none |


<a id="bzd_nodejs_binary"></a>

## bzd_nodejs_binary

<pre>
bzd_nodejs_binary(<a href="#bzd_nodejs_binary-name">name</a>, <a href="#bzd_nodejs_binary-main">main</a>, <a href="#bzd_nodejs_binary-args">args</a>, <a href="#bzd_nodejs_binary-visibility">visibility</a>, <a href="#bzd_nodejs_binary-tags">tags</a>, <a href="#bzd_nodejs_binary-kwargs">kwargs</a>)
</pre>



**PARAMETERS**


| Name  | Description | Default Value |
| :------------- | :------------- | :------------- |
| <a id="bzd_nodejs_binary-name"></a>name |  <p align="center"> - </p>   |  none |
| <a id="bzd_nodejs_binary-main"></a>main |  <p align="center"> - </p>   |  none |
| <a id="bzd_nodejs_binary-args"></a>args |  <p align="center"> - </p>   |  <code>[]</code> |
| <a id="bzd_nodejs_binary-visibility"></a>visibility |  <p align="center"> - </p>   |  <code>[]</code> |
| <a id="bzd_nodejs_binary-tags"></a>tags |  <p align="center"> - </p>   |  <code>[]</code> |
| <a id="bzd_nodejs_binary-kwargs"></a>kwargs |  <p align="center"> - </p>   |  none |


<a id="bzd_nodejs_docker"></a>

## bzd_nodejs_docker

<pre>
bzd_nodejs_docker(<a href="#bzd_nodejs_docker-name">name</a>, <a href="#bzd_nodejs_docker-deps">deps</a>, <a href="#bzd_nodejs_docker-cmd">cmd</a>, <a href="#bzd_nodejs_docker-base">base</a>, <a href="#bzd_nodejs_docker-include_metadata">include_metadata</a>, <a href="#bzd_nodejs_docker-deploy">deploy</a>)
</pre>



**PARAMETERS**


| Name  | Description | Default Value |
| :------------- | :------------- | :------------- |
| <a id="bzd_nodejs_docker-name"></a>name |  <p align="center"> - </p>   |  none |
| <a id="bzd_nodejs_docker-deps"></a>deps |  <p align="center"> - </p>   |  none |
| <a id="bzd_nodejs_docker-cmd"></a>cmd |  <p align="center"> - </p>   |  none |
| <a id="bzd_nodejs_docker-base"></a>base |  <p align="center"> - </p>   |  <code>"@docker_image_nodejs//image"</code> |
| <a id="bzd_nodejs_docker-include_metadata"></a>include_metadata |  <p align="center"> - </p>   |  <code>False</code> |
| <a id="bzd_nodejs_docker-deploy"></a>deploy |  <p align="center"> - </p>   |  <code>{}</code> |


<a id="bzd_nodejs_test"></a>

## bzd_nodejs_test

<pre>
bzd_nodejs_test(<a href="#bzd_nodejs_test-name">name</a>, <a href="#bzd_nodejs_test-main">main</a>, <a href="#bzd_nodejs_test-deps">deps</a>, <a href="#bzd_nodejs_test-visibility">visibility</a>, <a href="#bzd_nodejs_test-tags">tags</a>, <a href="#bzd_nodejs_test-kwargs">kwargs</a>)
</pre>



**PARAMETERS**


| Name  | Description | Default Value |
| :------------- | :------------- | :------------- |
| <a id="bzd_nodejs_test-name"></a>name |  <p align="center"> - </p>   |  none |
| <a id="bzd_nodejs_test-main"></a>main |  <p align="center"> - </p>   |  none |
| <a id="bzd_nodejs_test-deps"></a>deps |  <p align="center"> - </p>   |  <code>[]</code> |
| <a id="bzd_nodejs_test-visibility"></a>visibility |  <p align="center"> - </p>   |  <code>[]</code> |
| <a id="bzd_nodejs_test-tags"></a>tags |  <p align="center"> - </p>   |  <code>[]</code> |
| <a id="bzd_nodejs_test-kwargs"></a>kwargs |  <p align="center"> - </p>   |  none |


<a id="bzd_nodejs_deps_aspect"></a>

## bzd_nodejs_deps_aspect

<pre>
bzd_nodejs_deps_aspect(<a href="#bzd_nodejs_deps_aspect-name">name</a>)
</pre>



**ASPECT ATTRIBUTES**


| Name | Type |
| :------------- | :------------- |
| deps| String |


**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :------------- | :------------- | :------------- | :------------- | :------------- |
| <a id="bzd_nodejs_deps_aspect-name"></a>name |  A unique name for this target.   | <a href="https://bazel.build/concepts/labels#target-names">Name</a> | required |   |


