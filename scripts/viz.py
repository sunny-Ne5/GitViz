# visualization script using python
import pandas as pd
import matplotlib.pyplot as plt



report_path = ""
df = pd.read_csv(report_path, delimiter="\t\t", header=None, names=["Author", "Timestamp"])
unique_author_list = df['Author'].unique()
df["Timestamp"] = pd.to_datetime(df["Timestamp"], unit="s")
df.set_index('Timestamp', inplace=True)


commits_per_month = df.groupby('Author').resample('M').count()


# plot all the graphs

for author in unique_author_list:
    commits_per_month['Author'][author].plot(kind='line',  figsize=(10, 6))
    plt.title("Commits")
    plt.ylabel(author)









