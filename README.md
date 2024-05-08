
# TrackStar

<div align="center">
	<a href="https://github.com/giganano/TrackStar/actions/workflows/ci.yml"
		target="_blank">
		<img src="https://github.com/giganano/TrackStar/workflows/GitHub%20CI/badge.svg">
	</a>
	<a href="https://github.com/giganano/TrackStar/issues" target="_blank">
		<img src="https://img.shields.io/github/issues/giganano/TrackStar.svg">
	</a>
</div>

<div align="center">
	<a href="https://github.com/giganano/TrackStar/blob/main/LICENSE"
		target="_blank">
		<img src="https://img.shields.io/badge/License-MIT-blue.svg">
	</a>
</div>

TrackStar is a highly optimized, user-friendly library designed to compute
Bayesian likelihood estimates for arbitrary lines and curves when the data have
errors in every direction.
When combined with your favorite optimization routine (e.g. Markov chain Monte
Carlo, maximum a posteriori esimation), TrackStar provides blazing fast
best-fit parameters!

TrackStar users enjoy the following features:

<ul>
	<li>
		Support for samples in which some quantities are not available for
		every data vector.
		For example, if &theta;<sub>1</sub> and &theta;<sub>2</sub> are
		available for the whole sample, but only half of the data have
		measurements of &theta;<sub>3</sub>, TrackStar will automatically use
		&theta;<sub>3</sub> where it is available.
	</li>
	<li>
		Multi-threading with the
		<a href="https://openmp.org/" target="_blank">OpenMP</a> library.
		If these features are enabled when TrackStar is installed, simply
		tell it how many threads you'd like it to use, and hit run!
	</li>
	<li>
		Full control over the expected N-dimensional distribution of the data,
		both due to selection effects and arising from model predictions.
	</li>
</ul>

### Developers: To-Do Items

<ul>
	<li>
		Finish implementing unit tests
	</li>
	<li>
		Ensure the likelihood estimates is properly normalized when the data
		vectors do not have the same dimensionality (i.e. when not all
		quantities are measured for each datum).
	</li>
	<li>
		Finish writing documentation: API reference, science documentation,
		installation instructions, developers documentation, Changelog
	</li>
	<li>
		Write example codes
	</li>
</ul>

