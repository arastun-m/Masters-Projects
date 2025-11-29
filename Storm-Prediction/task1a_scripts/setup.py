#!/usr/bin/env python

from setuptools import setup, find_packages

setup(
    name="StormPredictionTool",  # Update with your project name
    version="1.0",
    description="A tool for storm-related analysis and predictions",
    author="ACDS Project Team Irma",  # Update with your team name or author
    packages=find_packages(), 
    install_requires=[
        "matplotlib",
        "numpy",
        "pandas",
        "folium",
        "scikit-learn",
        "scipy",
        "cartopy",  
        "Pillow",   
    ],
)