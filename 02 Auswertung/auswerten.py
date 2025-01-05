import pandas as pd
import matplotlib.pyplot as plt
import os

def create_boxplot_from_csv(file_path, column_name, group_column=None, output_file=None):
    """
    Erstellt einen Boxplot aus einer CSV-Datei.
    
    Args:
        file_path (str): Pfad zur CSV-Datei.
        column_name (str): Name der Spalte mit den numerischen Werten.
        group_column (str, optional): Name der Spalte zur Gruppierung. Default ist None.
        output_file (str, optional): Pfad zur Speicherung des Plots als Bilddatei. Default ist None.
    """
    # Validierung: Existiert die Datei?
    if not os.path.isfile(file_path):
        raise FileNotFoundError(f"Die Datei '{file_path}' wurde nicht gefunden.")
    
    # CSV-Datei einlesen
    try:
        df = pd.read_csv(file_path)
    except Exception as e:
        raise ValueError(f"Fehler beim Lesen der Datei: {e}")
    
    # Validierung: Existieren die angegebenen Spalten?
    if column_name not in df.columns:
        raise ValueError(f"Die Spalte '{column_name}' existiert nicht in der CSV-Datei.")
    if group_column and group_column not in df.columns:
        raise ValueError(f"Die Gruppenspalte '{group_column}' existiert nicht in der CSV-Datei.")
    
    # Gruppierung oder Einzel-Daten
    if group_column:
        grouped_data = df.groupby(group_column)[column_name].apply(list)
        data = grouped_data.values
        labels = grouped_data.index
    else:
        data = [df[column_name].dropna()]
        labels = [column_name]
    
    # Boxplot erstellen
    plt.figure(figsize=(10, 6))
    plt.boxplot(data, labels=labels, patch_artist=True, boxprops=dict(facecolor="lightblue"))
    plt.title("Boxplot", fontsize=16)
    plt.xlabel("Gruppen" if group_column else "Daten", fontsize=12)
    plt.ylabel(column_name, fontsize=12)
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    # Optional: Plot speichern
    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Boxplot wurde erfolgreich unter '{output_file}' gespeichert.")
    
    # Plot anzeigen
    plt.show()

# Beispiel-Aufruf
if __name__ == "__main__":
    try:
        # Anpassung an deine Datei und Spalten
        csv_file = "daten.csv"  # Datei anpassen
        value_column = "Werte"  # Spalte mit numerischen Werten
        group_column = "Gruppe"  # Optional: Gruppenspalte
        output_image = "boxplot.png"  # Optional: Bild speichern
        
        # Funktion aufrufen
        create_boxplot_from_csv(csv_file, value_column, group_column, output_image)
    except Exception as e:
        print(f"Fehler: {e}")
