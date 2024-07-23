
try:
	import pytest
except ModuleNotFoundError:
	pass
try:
	import numpy as np
except:
	pass


def test(*args, **kwargs):
	r"""
	Runs trackstar's unit tests.

	.. seealso:: ``pytest.main``

	Parameters
	----------
	*args
		Optional positional arguments to be passed to ``pytest.main``. See below.
	**kwargs
		Optional keyword arguments to be passed to ``pytest.main``. See below.

	Returns
	-------
	Returns the system exit code returned by ``pytest.main.``

	Notes
	-----
	This function does nothing more than invoke
	``pytest.main(["--pyargs", "trackstar"], \*args, \*\*kwargs)``.
	"""
	if "pytest" not in sys.modules: raise RuntimeError("""\
pytest is required to run TrackStar's unit tests (https://docs/pytest.org/).""")
	if "numpy" not in sys.modules: raise RuntimeError("""\
NumPy is required to run TrackStar's unit tests (https://numpy.org/).""")
	return pytest.main(["--pyargs", "trackstar"], *args, **kwargs)

