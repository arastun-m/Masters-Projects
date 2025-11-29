Installation Guide
=====================

Follow the steps below to set up the environment, install the module, and verify the project setup.

For Environment
--------------------

To set up the required environment, execute the following commands:

.. code-block:: bash

   conda env create -f environment.yml
   conda activate hygiea-env

This will create and activate a Conda environment named ``hygiea-env`` with the necessary dependencies specified in the ``environment.yml`` file.

Installation
-------------------

To install the module and any required dependencies, run the following commands from the base directory:

.. code-block:: bash

   pip install -r requirements.txt
   pip install -e .

The first command installs the dependencies listed in ``requirements.txt``, and the second command installs the module in editable mode.

Downloading Postcode Data
---------------------------

To download the required postcode data, use the following command:

.. code-block:: bash

   python download_data.py

This script will fetch and prepare the postcode data required by the project.

Automated Testing
---------------------

To ensure the project setup is correct and the code works as expected, run the pytest test suite from the base directory:

.. code-block:: bash

   pytest tests/



