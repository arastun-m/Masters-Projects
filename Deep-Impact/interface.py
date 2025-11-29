import streamlit as st
# import numpy as np
import matplotlib.pyplot as plt
import deepimpact
import pandas as pd
# import subprocess
from PIL import Image
from predict_parameters import main
from pres_mapper import create_map
import os


st.title("Deep Impact: The Hazard of Small Asteroids")
st.title("solver")
image_path1 = "uiPic/pre_pic.png"
if not os.path.exists(image_path1):
    raise FileNotFoundError(f"File not found: {image_path1}")
st.image(image_path1, caption="Planet Workflow")


image_path2 = "uiPic/UML.png"
if not os.path.exists(image_path2):
    raise FileNotFoundError(f"File not found: {image_path2}")
image2 = Image.open(image_path2)
original_width, original_height = image2.size
new_width = original_width // 3
st.image(image_path2, caption="Planet UML", width=new_width)


st.title("solve_atmospheric_entry")
# Load the Planet class
# try:
#     # Run the external Python script
#     result = subprocess.run(
#         ["python", "download_data.py"],
#         stdout=subprocess.PIPE,
#         stderr=subprocess.PIPE,
#         text=True,
#     )

#     # Display the script output
#     st.subheader("Script Output")
#     st.code(result.stdout)

#     # Display any errors
#     if result.stderr:
#         st.subheader("Errors")
#         st.code(result.stderr)
# except Exception as e:
#     st.error(f"An error occurred: {e}")

planet = deepimpact.Planet()
locator = deepimpact.GeospatialLocator()


# Function to plot results
def plot_results(data, title="Simulation Results"):
    fig, ax1 = plt.subplots(figsize=(8, 5))

    ax1.set_xlabel("Time (s)")
    ax1.set_ylabel("Altitude (m)", color="tab:blue")
    ax1.plot(
        data["time"], data["altitude"], label="Altitude", color="tab:blue")
    ax1.tick_params(axis="y", labelcolor="tab:blue")
    ax1.legend(loc="upper left")

    ax2 = ax1.twinx()
    ax2.set_ylabel("Velocity (m/s)", color="tab:red")
    ax2.plot(data["time"], data["velocity"], label="Velocity", color="tab:red")
    ax2.tick_params(axis="y", labelcolor="tab:red")
    ax2.legend(loc="upper right")

    plt.title(title)
    st.pyplot(fig)


def plot_results2(data, title="Simulation Results"):
    fig, ax1 = plt.subplots(figsize=(8, 5))

    ax1.set_xlabel("Time (s)")
    ax1.set_ylabel("Radius (km)", color="tab:blue")
    ax1.plot(data["time"], data["radius"], label="Radius", color="tab:blue")
    ax1.tick_params(axis="y", labelcolor="tab:blue")
    ax1.legend(loc="upper left")

    ax2 = ax1.twinx()
    ax2.set_ylabel("Angle (degree)", color="tab:red")
    ax2.plot(data["time"], data["angle"], label="Angle", color="tab:red")
    ax2.tick_params(axis="y", labelcolor="tab:red")
    ax2.legend(loc="upper right")

    plt.title(title)
    st.pyplot(fig)


def plot_results3(data, title="Simulation Results"):
    fig, ax1 = plt.subplots(figsize=(8, 5))

    ax1.set_xlabel("Time (s)")
    ax1.set_ylabel("Distance (m)", color="tab:blue")
    ax1.plot(
        data["time"], data["distance"], label="Distance", color="tab:blue")
    ax1.tick_params(axis="y", labelcolor="tab:blue")
    ax1.legend(loc="upper left")

    ax2 = ax1.twinx()
    ax2.set_ylabel("Mass ()", color="tab:red")
    ax2.plot(data["time"], data["mass"], label="Mass", color="tab:red")
    ax2.tick_params(axis="y", labelcolor="tab:red")
    ax2.legend(loc="upper right")

    plt.title(title)
    st.pyplot(fig)


def plot_results4(result, energies):
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(energies['dedz'], result['altitude'])
    ax.set_xlabel('Energy lost per unit altitude (Kt/m)')
    ax.set_ylabel('Altitude (m)')
    ax.set_title('Energy lost per unit altitude vs Altitude')
    st.pyplot(fig)


# Streamlit UI
st.title("Asteroid Impact Simulation")

st.sidebar.header("Input Parameters")
radius = st.sidebar.number_input(
    "Meteoroid Radius (m)", value=40.0, min_value=0.1)
velocity = st.sidebar.number_input(
    "Meteoroid Velocity (m/s)", value=17000.0, min_value=1.0)
density = st.sidebar.number_input(
    "Meteoroid Density (kg/m³)", value=2850.0, min_value=1.0)
strength = st.sidebar.number_input(
    "Meteoroid Strength (Pa)", value=4000000.0, min_value=1.0)
angle = st.sidebar.number_input(
    "Meteoroid Angle (degrees)", value=30.0, min_value=1.0, max_value=90.0)
dt = st.sidebar.number_input(
    "Time Step (s)", value=0.25, min_value=0.01)
lat = st.sidebar.number_input(
    "latitude of entry point (degrees)", value=52.03, min_value=1.0)
lon = st.sidebar.number_input(
    "longitude of entry point (degrees)", value=0.21, min_value=-1000.0)
bearing = st.sidebar.number_input(
    "Bearing of meteoroid trajectory (degrees)", value=290.0, min_value=0.01)

# Run simulation on button click

st.write("Running simulation with the following inputs:")
st.write(f"Radius: {radius} m")
st.write(f"Velocity: {velocity} m/s")
st.write(f"Density: {density} kg/m³")
st.write(f"Strength: {strength} Pa")
st.write(f"Angle: {angle} degrees")
st.write(f"Time Step: {dt} s")

# Call the solve_atmospheric_entry function
result = planet.solve_atmospheric_entry(
    radius=radius,
    velocity=velocity,
    density=density,
    strength=strength,
    angle=angle,
    dt=dt
)

result2 = planet.calculate_energy(result)
energies = planet.calculate_energy(result2)
outcome = planet.analyse_outcome(result2)

# Display results as a table
st.write("### Simulation Results")
st.dataframe(result)

# Plot results
plot_results(
    result, title="Asteroid Simulation: Altitude and Velocity vs Time")
plot_results2(
    result, title="Asteroid Simulation: Radius and Angle vs Time")
plot_results3(
    result, title="Asteroid Simulation: Mass and Diatance vs Time")

# Allow download of results
csv = result.to_csv(index=False).encode("utf-8")
st.download_button(
    label="Download Results as CSV",
    data=csv,
    file_name="simulation_results.csv",
    mime="text/csv",
)

image_path3 = "uiPic/solver1.png"
if not os.path.exists(image_path3):
    raise FileNotFoundError(f"File not found: {image_path3}")
image3 = Image.open(image_path3)
original_width, original_height = image3.size
new_width = original_width // 3
st.image(image_path3, caption="Planet UML", width=new_width)

st.title("calculate_energy")
st.dataframe(result2)
plot_results4(result2, energies)

st.title("analyse_outcome")
# Display outcome
st.write("Event Type")
st.subheader(f"{outcome['outcome']}")
st.metric(
    label="Peak Energy Deposition (kt/km)",
    value=f"{outcome['burst_peak_dedz']:.3f}"
)
st.metric(
    label="Burst Altitude (m)",
    value=f"{outcome['burst_altitude']:.2f}"
)
st.metric(
    label="Burst Distance (m)",
    value=f"{outcome['burst_distance']:.2f}"
)
st.metric(
    label="Burst Energy (kt)",
    value=f"{outcome['burst_energy']:.2f}"
)

# Alternatively, display as a table
st.subheader("Detailed Data")
formatted_data = {
    "Metric": [
        "Peak Energy Deposition (kt/km)",
        "Burst Altitude (m)",
        "Burst Distance (m)",
        "Burst Energy (kt)"
    ],
    "Value": [
        f"{outcome['burst_peak_dedz']:.3f}",
        f"{outcome['burst_altitude']:.2f}",
        f"{outcome['burst_distance']:.2f}",
        f"{outcome['burst_energy']:.2f}"
    ]
}
st.table(formatted_data)

image_path4 = "uiPic/otms1.png"
if not os.path.exists(image_path4):
    raise FileNotFoundError(f"File not found: {image_path4}")
image3 = Image.open(image_path4)
original_width, original_height = image3.size
new_width = original_width // 1
st.image(
    image_path4,
    caption=(
        "plot of polynomial interpolation curve fitted "
        "(deg 6) against data"
    ),
    width=new_width
)

image_path5 = "uiPic/otms2.png"
if not os.path.exists(image_path5):
    raise FileNotFoundError(f"File not found: {image_path5}")
image3 = Image.open(image_path5)
original_width, original_height = image3.size
new_width = original_width // 1
st.image(
    image_path5,
    caption=(
        "plot of piece-wise polynomial interpolation and "
        "exponential data"
    ),
    width=new_width
)

image_path6 = "uiPic/otms3.png"
if not os.path.exists(image_path6):
    raise FileNotFoundError(f"File not found: {image_path6}")
image3 = Image.open(image_path6)
original_width, original_height = image3.size
new_width = original_width // 1
st.image(
    image_path6,
    caption="Fix to the problem + updated plot",
    width=new_width
)

input_csv_path = "resources/ChelyabinskEnergyAltitude.csv"

st.title("Extension Functionality: determine impactor parameters")
st.write("(i.e. strength, radius and pancake factor) \
         that best fit an observed energy deposition curve")
st.write("Example input: Chelyabinsk event")
st.write("Optimized Parameters:")
# Call the main function and get results

radius, strength, pancake_factor = main(input_csv_path)
st.metric(
    label="Radius",
    value=f"{radius:.2f} m"
)
st.metric(
    label="Strength",
    value=f"{strength:.2f} m"
)
st.metric(
    label="Pancake Factor",
    value=f"{pancake_factor:.2f} m"
)


st.title("mapper")

image_path7 = "uiPic/mapper_workflow.png"
if not os.path.exists(image_path7):
    raise FileNotFoundError(f"File not found: {image_path7}")
image3 = Image.open(image_path7)
original_width, original_height = image3.size
new_width = original_width // 4
st.image(image_path7, caption="mapper_workflow", width=new_width)

image_path8 = "uiPic/mapper_UML.png"
if not os.path.exists(image_path8):
    raise FileNotFoundError(f"File not found: {image_path8}")
image3 = Image.open(image_path8)
original_width, original_height = image3.size
new_width = original_width // 4
st.image(image_path8, caption="mapper_UML", width=new_width)

pressures = [1e3, 5e3, 25e3, 40e3]

blast_lat, blast_lon, damage_rad = deepimpact.damage_zones(
    outcome, lat, lon, bearing, pressures=pressures)

st.metric(label="blast_lat", value=f"{blast_lat:.3f}")
st.metric(label="blast_lon", value=f"{blast_lon:.2f}")

# Display data as a list
st.write("damage_rad")
st.write(damage_rad)

# Display data as a table
data_table = pd.DataFrame(
    {"Index": range(1, len(damage_rad) + 1), "Value": damage_rad})
st.table(data_table)

# Display data as a bar chart
st.bar_chart(damage_rad)

# Display data as a line chart
fig, ax = plt.subplots()
ax.plot(range(1, len(damage_rad) + 1), damage_rad, marker='o')
ax.set_title("Line Chart of Data")
ax.set_xlabel("Index")
ax.set_ylabel("Value")
st.pyplot(fig)

damage_map = deepimpact.plot_circle(blast_lat, blast_lon, damage_rad,)
damage_map.save("damage_map.html")

# Streamlit app
st.title("Damage Map")
try:
    # Load the saved HTML file and display it in an iframe
    with open("damage_map.html", "r", encoding="utf-8") as f:
        html_content = f.read()
    st.components.v1.html(html_content, height=600, scrolling=True)
except FileNotFoundError:
    st.error("HTML file not found! Please generate the map first.")

# Find the postcodes in the damage radii
postcodes = locator.get_postcodes_by_radius(
    (blast_lat, blast_lon), radii=damage_rad
)

# Find the population in each postcode
population = locator.get_population_by_radius(
    (blast_lat, blast_lon), radii=damage_rad
)

# Prepare the data for display
data = {
    "Pressure (kPa)": [f"{p / 1e3:.0f}" for p in pressures],  # kPa
    "Damage Radius (km)": [f"{r / 1e3:.1f}" for r in damage_rad],  # km
    "Population Affected": [f"{pop:,}" for pop in population]
}
df = pd.DataFrame(data)

# Streamlit display
st.title("Damage Zone Analysis")

# Display as a styled table
st.subheader("Population Affected by Damage Zones")
st.table(df)

# Display data as a bar chart
data = {
    "Radius (km)": damage_rad,
    "Population": population
}
df = pd.DataFrame(data)
st.subheader("Bar Chart of Population by Radius")
st.bar_chart(df.set_index("Radius (km)"))

# Optionally, show the raw data
st.subheader("Raw Data")
st.write({
    "Pressure (kPa)": pressures,
    "Damage Radius (m)": damage_rad,
    "Population": population
})

st.subheader("First 50 Postcodes in Each Damage Zone")
limited_postcodes_by_zone = {
    f"Zone {i+1} (Radius: {damage_rad[i] / 1e3:.1f} km)": postcodes[i][:50]
    for i in range(len(postcodes))
}
# Display the postcodes
for zone, limited_postcodes in limited_postcodes_by_zone.items():
    st.write(f"**{zone}**")
    st.write(", ".join(limited_postcodes))

# Optionally, display detailed postcodes for all zones
# st.subheader("Postcodes by Damage Zone")
# detailed_postcodes = {
#     f"Zone {i+1} (Radius: {damage_rad[i] / 1e3:.1f} km)": postcodes[i]
#     for i in range(len(postcodes))
# }
# st.json(detailed_postcodes)

map1 = create_map(True)
map2 = create_map()
map1.save("map1.html")
map2.save("map2.html")

try:
    # Load the saved HTML file and display it in an iframe
    with open("map1.html", "r", encoding="utf-8") as f:
        html_content = f.read()
    st.components.v1.html(html_content, height=600, scrolling=True)
except FileNotFoundError:
    st.error("HTML file not found! Please generate the map first.")

try:
    # Load the saved HTML file and display it in an iframe
    with open("map2.html", "r", encoding="utf-8") as f:
        html_content = f.read()
    st.components.v1.html(html_content, height=600, scrolling=True)
except FileNotFoundError:
    st.error("HTML file not found! Please generate the map first.")

probability, population = deepimpact.impact_risk(planet, pressure=30e3)

# Streamlit App
st.title("Impact Risk Analysis")

# Display the probability DataFrame
st.subheader("Probability of Damage by Postcode")
st.write("Here is a preview of the probability data:")

# Display 10 random rows
if len(probability) >= 10:
    random_sample = probability.sample(10)  # Get 10 random rows
else:
    random_sample = probability  # Show all rows if less than 10

# Show the random sample in the app
st.table(random_sample)

# # Option to refresh random rows
# if st.button("Show another 10 random rows"):
#     random_sample = probability.sample(10)
#     st.table(random_sample)

# Add search functionality for a specific postcode
st.subheader("Search Probability by Postcode")

postcode_to_search = st.text_input("Enter the postcode to search:")

# Display the population statistics
st.subheader("Population Impact Summary")
st.metric(
    label="Total Population Affected (Mean)",
    value=f"{population['mean']:,.0f}"
)
st.metric(
    label="Standard Deviation",
    value=f"{population['stdev']:,.0f}"
)
# x = st.slider("choose a value", 0, 100, 50)
# st.write(f"{x}")

# values = np.linspace(0, x, 100)
# plt.plot(values, values**2)
# st.pyplot(plt)

# Example Data
postcode_data = {
    "Postcode": [
        "B92 8HX", "B92 8LP", "B26 3LY", "B26 3PN", "B26 3SJ",
        "B26 3NE", "B26 3LZ", "B26 3TB", "B92 9BG", "B26 3LS",
        "B92 8JG", "B26 3LH", "B92 9AH", "B26 3JS"
    ],
    "Probability": [
        0.275, 0.275, 0.275, 0.275, 0.274, 0.274, 0.274, 0.274, 0.274,
        0.274, 0.274, 0.274, 0.274, 0.274
    ]
}

time_data = {
    "N samples": [10, 100, 1000, 10000],
    "Time (s)": [45.47, 450.81, 4555.91, 45555.92]
}

# Convert to DataFrames
postcode_df = pd.DataFrame(postcode_data)
time_df = pd.DataFrame(time_data)

# Streamlit App
st.title("Streamlit Data Display Example")

# Section 1: Display Postcode Probabilities
st.subheader("Postcode Probabilities")
st.table(postcode_df)  # Display the table

# Section 2: Time Taken for Different Sample Sizes
st.subheader("Time Taken for Different Sample Sizes")
st.table(time_df)

# Optional: Visualize the time data
st.subheader("Time Analysis by Number of Samples")
st.line_chart(time_df.set_index("N samples"))  # Plot the time data
