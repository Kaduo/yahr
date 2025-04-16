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

def crop_times_to_problem(name, problem_id, haptics):
    haptics_folder = "haptics" if haptics else "no_haptics"
    eye_tracking = pl.read_csv(f"data/experiment/results/{haptics_folder}/{name}/{name}.csv")
    eye_tracking = eye_tracking.with_columns((eye_tracking["Timestamp (ms)"]/1000).alias("Timestamp (s)"))
    start = get_start_time_of_tap(f"data/experiment/results/{haptics_folder}/{name}/problem_{problem_id}.tap")
    end = get_end_time_of_tap(f"data/experiment/results/{haptics_folder}/{name}/problem_{problem_id}.tap")
    print(end - start)
    df = eye_tracking.filter((eye_tracking["Timestamp (s)"] <= end) & (start <= eye_tracking["Timestamp (s)"]))
    df = df.with_columns((df["Timestamp (s)"] - df["Timestamp (s)"][0]).alias("Elapsed time (s)"))
    return df


if __name__ == "__main__":
    name = "louis"
    problem_id = 12
    fig = px.line(crop_times_to_problem(name, problem_id), x="Elapsed time (s)", y="Pitch")
    print(crop_times_to_problem(name, problem_id))
    fig.show()
    # eye_tracking = pl.read_csv(f"data/experiment/results/{name}/{name}.csv")
    # print((eye_tracking["Timestamp (ms)"].max() - eye_tracking["Timestamp (ms)"].min())/(1000*60))
    # print(eye_tracking["Timestamp (ms)"].min()/1000)
    # fig = px.line(eye_tracking, x="Timestamp (ms)", y="Pitch")
    # start = None
    # for i in range(0, 20):
    #     start = get_start_time_of_tap(f"data/experiment/results/{name}/problem_{i}.tap")
    #     end = get_end_time_of_tap(f"data/experiment/results/{name}/problem_{i}.tap")

    #     if start is not None and end is not None:
    #         color = "red"
    #         if i % 2 == 0:
    #             color = "green"
    #         fig.add_vline(x=start*1000, line={"color": color})
    #         fig.add_vline(x=end*1000, line={"color": color})

    # fig.show()