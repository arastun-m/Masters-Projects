from setuptools import setup, find_packages

setup(
    name="StormPredictionTool",  # Project name
    version="1.0",
    description="A tool for storm-related analysis and predictions",
    author="ACDS Project Team Irma",  # Update with your team name or author
    packages=find_packages(),  # Automatically find all packages
    install_requires=[
        "matplotlib",
        "numpy",
        "pandas",
        "folium",
        "scikit-learn",
        "scipy",
        "cartopy",
        "Pillow",
        "tensorflow",
        "h5py",
    ],
    include_package_data=True,
)
