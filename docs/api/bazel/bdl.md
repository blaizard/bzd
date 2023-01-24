<!-- Generated with Stardoc: http://skydoc.bazel.build -->



<a id="bdl_composition"></a>

## bdl_composition

<pre>
bdl_composition(<a href="#bdl_composition-name">name</a>, <a href="#bdl_composition-deps">deps</a>)
</pre>

Bzd Description Language generator rule for binaries.
    This generates all the glue to pull in bdl pieces together in a final binary.
    

**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :------------- | :------------- | :------------- | :------------- | :------------- |
| <a id="bdl_composition-name"></a>name |  A unique name for this target.   | <a href="https://bazel.build/concepts/labels#target-names">Name</a> | required |  |
| <a id="bdl_composition-deps"></a>deps |  List of dependencies.   | <a href="https://bazel.build/concepts/labels">List of labels</a> | required |  |


<a id="bdl_library"></a>

## bdl_library

<pre>
bdl_library(<a href="#bdl_library-name">name</a>, <a href="#bdl_library-deps">deps</a>, <a href="#bdl_library-srcs">srcs</a>)
</pre>

Bzd Description Language generator rule.
    It generates language provider from a .bdl file.
    The files are generated at the same path of the target, with the name of the target appended with a language specific file extension.
    

**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :------------- | :------------- | :------------- | :------------- | :------------- |
| <a id="bdl_library-name"></a>name |  A unique name for this target.   | <a href="https://bazel.build/concepts/labels#target-names">Name</a> | required |  |
| <a id="bdl_library-deps"></a>deps |  List of bdl dependencies. Language specific dependencies will have their public interface included in the generated file.   | <a href="https://bazel.build/concepts/labels">List of labels</a> | optional | <code>[]</code> |
| <a id="bdl_library-srcs"></a>srcs |  List of Bzd Description Language (bdl) files to be included.   | <a href="https://bazel.build/concepts/labels">List of labels</a> | required |  |


<a id="BdlProvider"></a>

## BdlProvider

<pre>
BdlProvider(<a href="#BdlProvider-bdls">bdls</a>, <a href="#BdlProvider-files">files</a>)
</pre>



**FIELDS**


| Name  | Description |
| :------------- | :------------- |
| <a id="BdlProvider-bdls"></a>bdls |  (Undocumented)    |
| <a id="BdlProvider-files"></a>files |  (Undocumented)    |


<a id="BdlTagProvider"></a>

## BdlTagProvider

<pre>
BdlTagProvider()
</pre>



**FIELDS**



<a id="CcCompositionProvider"></a>

## CcCompositionProvider

<pre>
CcCompositionProvider(<a href="#CcCompositionProvider-hdrs">hdrs</a>)
</pre>



**FIELDS**


| Name  | Description |
| :------------- | :------------- |
| <a id="CcCompositionProvider-hdrs"></a>hdrs |  (Undocumented)    |


