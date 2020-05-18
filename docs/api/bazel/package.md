<!-- Generated with Stardoc: http://skydoc.bazel.build -->

<a name="#bzd_package"></a>

## bzd_package

<pre>
bzd_package(<a href="#bzd_package-name">name</a>, <a href="#bzd_package-deps">deps</a>)
</pre>



**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :-------------: | :-------------: | :-------------: | :-------------: | :-------------: |
| name |  A unique name for this target.   | <a href="https://bazel.build/docs/build-ref.html#name">Name</a> | required |  |
| deps |  Target or files dependencies to be added to the package.   | <a href="https://bazel.build/docs/skylark/lib/dict.html">Dictionary: Label -> String</a> | optional | {} |


<a name="#BzdPackageFragment"></a>

## BzdPackageFragment

<pre>
BzdPackageFragment(<a href="#BzdPackageFragment-root">root</a>, <a href="#BzdPackageFragment-files">files</a>, <a href="#BzdPackageFragment-files_remap">files_remap</a>, <a href="#BzdPackageFragment-tars">tars</a>)
</pre>



**FIELDS**


| Name  | Description |
| :-------------: | :-------------: |
| root |  (Undocumented)    |
| files |  (Undocumented)    |
| files_remap |  (Undocumented)    |
| tars |  (Undocumented)    |


<a name="#BzdPackageMetadataFragment"></a>

## BzdPackageMetadataFragment

<pre>
BzdPackageMetadataFragment(<a href="#BzdPackageMetadataFragment-manifests">manifests</a>)
</pre>



**FIELDS**


| Name  | Description |
| :-------------: | :-------------: |
| manifests |  (Undocumented)    |


