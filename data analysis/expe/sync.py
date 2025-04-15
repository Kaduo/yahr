import polars as pl
import plotly.express as px

def get_start_time_of_tap(filename):
    with open(filename, "r") as f:
        return int(f.readline())
    
def get_end_time_of_tap(filename):
    start = get_start_time_of_tap(filename)
    rel_start = None
    rel_end = None

    with open(filename, "r") as f:
        for line in f:
            line = line.split()
            if line[0] == "time":
                rel_start = float(line[1])
                break
        for line in f:
            line = line.split()
            if line[0] == "time":
                rel_end = float(line[1])
    
    if rel_end is not None:
        return start + rel_end - rel_start
    else:
        return None


if __name__ == "__main__":
    name = "ecma1"

    eye_tracking = pl.read_csv(f"data/experiment/results/{name}/{name}.csv")
    print((eye_tracking["Timestamp (ms)"].max() - eye_tracking["Timestamp (ms)"].min())/(1000*60))
    print(eye_tracking["Timestamp (ms)"].min()/1000)
    fig = px.line(eye_tracking, x="Timestamp (ms)", y="Pitch")
    start = None
    for i in range(0, 20):
        start = get_start_time_of_tap(f"data/experiment/results/{name}/problem_{i}.tap")
        end = get_end_time_of_tap(f"data/experiment/results/{name}/problem_{i}.tap")

        if start is not None and end is not None:
            fig.add_vline(x=start*1000, line={"color": "red"})
            fig.add_vline(x=end*1000, line={"color": "green"})

    fig.show()