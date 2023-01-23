<!-- Generated with Stardoc: http://skydoc.bazel.build -->



<a id="bzd_docker_load"></a>

## bzd_docker_load

<pre>
bzd_docker_load(<a href="#bzd_docker_load-name">name</a>, <a href="#bzd_docker_load-digest">digest</a>, <a href="#bzd_docker_load-registry">registry</a>, <a href="#bzd_docker_load-repository">repository</a>)
</pre>

Load a docker image.

**ATTRIBUTES**


| Name  | Description | Type | Mandatory | Default |
| :------------- | :------------- | :------------- | :------------- | :------------- |
| <a id="bzd_docker_load-name"></a>name |  A unique name for this target.   | <a href="https://bazel.build/concepts/labels#target-names">Name</a> | required |  |
| <a id="bzd_docker_load-digest"></a>digest |  The digest of the image to pull.   | String | optional | <code>""</code> |
| <a id="bzd_docker_load-registry"></a>registry |  The registry from which we are pulling.   | String | required |  |
| <a id="bzd_docker_load-repository"></a>repository |  The name of the image.   | String | required |  |


<a id="bzd_docker_pull"></a>

## bzd_docker_pull

<pre>
bzd_docker_pull(<a href="#bzd_docker_pull-kwargs">kwargs</a>)
</pre>

Pull a docker image locally.

**PARAMETERS**


| Name  | Description | Default Value |
| :------------- | :------------- | :------------- |
| <a id="bzd_docker_pull-kwargs"></a>kwargs |  <p align="center"> - </p>   |  none |


