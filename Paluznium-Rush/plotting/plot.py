import diagrams as dia
from diagrams import Diagram, Node, Edge
from diagrams.custom import Custom
import json
import os

def get_recoveries(pal_mf, gor_mf, tail_mf, mfrs):
    """
    Get the recoveries of the concentrate streams where the recovery is based on the mass flow rate of the stream. 
    """
    pal_recovery = pal_mf / mfrs[0]
    gor_recovery = gor_mf / mfrs[1]
    tail_recovery = tail_mf / mfrs[2]

    return pal_recovery, gor_recovery, tail_recovery

def get_recovery_opacity(recovery, max_recovery, min_recovery):
    """
    Get the opacity for RGBA color of the concentrate and tailing streams where the opacity 
    is based on the recovery of the stream. 
    """
    
    # Normalize the recovery to range 0-1 based on min and max recoveries
    normalized_recovery = (recovery - min_recovery) / (max_recovery - min_recovery)
    
    # Convert to hex opacity
    min_opacity = 64   # 40 in hex
    max_opacity = 255  # FF in hex
    opacity_range = max_opacity - min_opacity
    
    opacity_value = int(min_opacity + (normalized_recovery * opacity_range))
    
    # Convert to hex string, ensuring 2 digits
    return format(opacity_value, '02X')
        
if __name__ == "__main__":
    # Read data from json
    with open("plotting/data/unit_data.json", "r") as f:
        unit_data = json.load(f)

    with open("plotting/data/performance_data.json", "r") as f:
        performance_data = json.load(f)

    # Get plotting variables
    save_title = performance_data["name"]
    system_vector = performance_data["circuit_vector"]
    units = len(system_vector) // 2
    mfrs = performance_data["feed"] # Mass flow rates of the feed streams
    return_rate = performance_data["profit"]
    pal_rec = performance_data["recoveries"]["palusznium"]
    gor_rec = performance_data["recoveries"]["gormanium"]
    pal_grade = performance_data["grades"]["palusznium"]
    gor_grade = performance_data["grades"]["gormanium"]

    # Get recoveries across units
    pal_recoveries, gor_recoveries, tail_recoveries = [], [], []
    for unit in range(units):
        pal_recovery, gor_recovery, tail_recovery = get_recoveries(
            unit_data["units"][unit]["concentrate"]["pal"], 
            unit_data["units"][unit]["concentrate"]["gor"],
            unit_data["units"][unit]["tailings"]["waste"], mfrs)
        pal_recoveries.append(pal_recovery)
        gor_recoveries.append(gor_recovery)
        tail_recoveries.append(tail_recovery)

    # Get min and max recoveries for scaling opacity 
    avg_recoveries = [(pal_r + gor_r) / 2 for pal_r, gor_r in zip(pal_recoveries, gor_recoveries)]
    max_conc_recovery = max(avg_recoveries)
    min_conc_recovery = min(avg_recoveries)
    max_tail_recovery = max(tail_recoveries)
    min_tail_recovery = min(tail_recoveries)

    # Create title and subtitle to fit diagram format 
    title = f"System Vector: {system_vector}"
    subtitle = f"This circuit returns £{return_rate:.2f} per second, with a {round(pal_rec*100)}% Paluznium recovery at grade {round(pal_grade*100)}%, and a {round(gor_rec*100)}% Gormanium recovery at grade {round(gor_grade*100)}%"
    combined_title = f"{title}\n\n{subtitle}"

    # Create Diagram
    with Diagram(save_title, show=False, direction="LR", 
        filename=f"./plotting/plots/{save_title}",
        graph_attr={
            "splines": "curve",
            "nodesep": "0.6", 
            "ranksep": "0.8",
            "overlap": "false",
            "pad": "0.5", 
            "label": combined_title,       
            "labelloc": "t",     
            "labeljust": "c",    
            "fontsize": "15",     
        }) as diag:

        # create nodes
        nodes = [Custom(f"Unit {i}", os.path.join(os.path.dirname(__file__), "Icons", "unit.png")) for i in range(units)]
        feed = Custom("Feed", os.path.join(os.path.dirname(__file__), "Icons", "feed.png"))
                
        # Create a subgraph for the fixed nodes
        with dia.Cluster("Products", graph_attr={"rank": "same"}):
            palu = Custom("Palusznium", os.path.join(os.path.dirname(__file__), "Icons", "pal.png"))
            gorm = Custom("Gormanium", os.path.join(os.path.dirname(__file__), "Icons", "gor.png"))
            tail = Custom("Tailings", os.path.join(os.path.dirname(__file__), "Icons", "tail.png"))

        nodes.append(palu) # Unit n
        nodes.append(gorm) # Unit n + 1
        nodes.append(tail) # Unit n + 2

        # Connect feed to first unit
        feed >> Edge(tailport="e", headport="w") >> nodes[system_vector[0]]

        # Connect units based on system vector
        for unit in range(units):
            conc_dest = system_vector[2*unit + 1]
            tail_dest = system_vector[2*unit + 2]

            conc_opacity = get_recovery_opacity(avg_recoveries[unit], max_conc_recovery, min_conc_recovery)
            tail_opacity = get_recovery_opacity(tail_recoveries[unit], max_tail_recovery, min_tail_recovery)
            conc_color = f"#0000FF{conc_opacity}"
            conc_label = f"Pal: {pal_recoveries[unit]*100:.1f}%\nGor: {gor_recoveries[unit]*100:.1f}%"
            tail_color = f"#FF0000{tail_opacity}"
            tail_label = f"Tail: {tail_recoveries[unit]*100:.1f}%"

            # concentrate edge
            nodes[unit] >> Edge(color=conc_color, penwidth="4.0",
                tailport="e", headport="w",
                fontsize="10.0",
                label=conc_label
            ) >> nodes[conc_dest]

            # tailings edge
            nodes[unit] >> Edge(color=tail_color, penwidth="4.0",
                                tailport="e", headport="w",
                                fontsize="10.0",
                                label=tail_label) >> nodes[tail_dest]

    diag.render()
