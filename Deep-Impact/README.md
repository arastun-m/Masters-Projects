# Deep Impact: The Hazard of Small Asteroids

## Create environment
```
conda env create -f environment.yml
conda activate hygiea-env
```

## Installation

To install the module and any pre-requisites, from the base directory run
```
pip install -r requirements.txt
pip install -e .
```  

## Downloading postcode data

To download the postcode data
```
python download_data.py
```

## Automated testing
We prioritize testing to ensure code stability and reliability. Our automated test suite runs with every change, catching potential issues early. This continuous testing approach guarantees that our project remains robust and maintainable over time.

To run the pytest test suite (locally/manually), from the base directory run
```
pytest tests/
```

## Documentation
We maintain clear, comprehensive documentation for all code, using tools like Sphinx for consistency and accessibility. Our user guides and examples are regularly updated to ensure new contributors can quickly understand and engage with the project.

Documentation in html format can be found in `docs` folder.

To generate locally/manually, from the base directory run
```
python -m sphinx docs html
```
The HTML documentation includes the landing page, Installation Guide, Quick Start Guide, and API Reference. You can access the HTML documentation for our project by running the following command:
```
open build/html/index.html
```
When pushing to main or making a PR to the main branch, a new version of the documentation will be automatically built and uploaded if there are changes to the .py, .rst or .ipynb files.

## User Interface
To showcase the features of our project in a clear and intuitive way, we have designed a user-friendly and visually appealing interface. You can access the project's UI by running the following command.
```
streamlit run interface.py
```

In addition to displaying the project workflow diagram, users can modify the input parameters for the simulation directly on the page. These parameters include: Meteoroid Radius (m), Meteoroid Velocity (m/s), Meteoroid Density (kg/m³), Meteoroid Strength (Pa), Meteoroid Angle (degrees), Time Step (s), Latitude of Entry Point (degrees), Longitude of Entry Point (degrees), and Bearing of Meteoroid Trajectory (degrees). After clicking the "Run Simulation" button, the page will automatically run and display the computed results for each function.

## Github Usage
We follow best practices to ensure smooth collaboration and high-quality code:
- Branching Strategy: A structured branching model helps keep development organized.
- PEP 8 Compliance, Testing and Build documentation: Python PEP 8 linting and a run-tests workflows are implemented to ensure code adheres to style standards and functions as expected. Build documentation workflow is implemented to automatically update the new version of documentation if there are changes to the .py, .rst or .ipynb files.
- Code Review Requirements: Every change is reviewed by at least two team members before merging into the main, solver, and mapper branches.
- Pull Request Template: A predefined pull request template is enforced to ensure all changes are properly tested and documented before merging.
- Issue Tracking: Tasks and bugs are tracked through GitHub Issues for transparency.
- GitHub Projects: We use GitHub Projects to manage issues, pull requests, and task delegation, providing a clear view of ongoing work and priorities.

By adopting these practices, we ensure our project remains sustainable, organized, and efficient.

## Example usage

For example usage see `example.py` in the examples folder:
```
python examples/example.py
```

## Extenstion

We implemented the second extension of the solver in *predict_parameters.py*. We optimised the parameters (radius, strength, and pancake factor) for simulating the energy deposition (dedz) during the Chelyabinsk atmospheric entry event. Using the **Nelder-Mead** optimization method, which is well-suited for noisy, non-differentiable, or gradient-less objective functions, we minimised the sum of squared differences between observed and simulated Energy Per Unit Length values. The optimization is performed on initial guesses for the parameters, with bounds for radius (5-15 m), strength (0.1-10 MPa), and pancake factor (2-10). The optimised parameters are determined through iterative adjustment without requiring gradient calculations.

## Highlights

Bisection method for nonlinear solver:

For the nonlinear solver used to solve r, we use the bisection method. The advantage of this method is that it only needs to define the positive and negative values of the function and constantly shorten the range, without relying on derivatives or complex initial estimates. Moreover, this method only requires that the objective function is continuous over a given range and that the range contains a root (i.e., the function values of the two endpoints are opposite in sign). This guarantees that the algorithm will always find a solution. Before using this Method, we tried to use Newton's method, but with this method we got stuck in the local extremum, because after running the error derivative was 0. The bisection method solves this problem perfectly by ensuring that it does not fall into local extreme values while maintaining efficiency. This nonlinear solver has high accuracy on test data.
 
Bounding box for postcode and population:

When solving postcode and population in burst range, we create a bounding box with the maximum and minimum latitude and longitude of the burst range. Data beyond this box will not be considered, while data within this box will be judged whether it is within the explosion range according to the distance between the formula calculation and the surface zero location, thus greatly improving the efficiency of the algorithm. The high efficiency and accuracy of the test data prove the feasibility of this method.

## More information

For more information on the project specfication, see the python notebooks: `ProjectDescription.ipynb`, `AirburstSolver.ipynb` and `DamageMapper.ipynb`.

## License

This project is licensed under the GNU General Public License v3.0. You can freely use, modify, and distribute the code, provided that any derivative works are also licensed under the same terms. See the LICENSE file for more details.