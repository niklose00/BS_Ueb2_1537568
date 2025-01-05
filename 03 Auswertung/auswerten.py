import pandas as pd
import matplotlib.pyplot as plt
import os

def create_boxplot_from_csv(file_path, output_file=None):
    """
    Erstellt einen Boxplot aus einer CSV-Datei mit einer einzigen Spalte.
    
    Args:
        file_path (str): Pfad zur CSV-Datei.
        output_file (str, optional): Pfad zur Speicherung des Plots als Bilddatei. Default ist None.
    """
    # Validierung: Existiert die Datei?
    if not os.path.isfile(file_path):
        raise FileNotFoundError(f"Die Datei '{file_path}' wurde nicht gefunden.")
    
    # CSV-Datei einlesen (ohne Spaltenüberschrift)
    try:
        data = pd.read_csv(file_path, header=None).squeeze()
    except Exception as e:
        raise ValueError(f"Fehler beim Lesen der Datei: {e}")
    
    # Daten bereinigen (Nur numerische Werte)
    data = pd.to_numeric(data, errors="coerce").dropna()
    
    # Boxplot erstellen
    plt.figure(figsize=(8, 5))
    plt.boxplot(data, patch_artist=True, boxprops=dict(facecolor="lightblue"))
    plt.title("Boxplot", fontsize=16)
    plt.ylabel("Werte", fontsize=12)
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
        # Anpassung an deine Datei
        csv_file = "02_Outputs/01_spinlock_latencies.csv"  # Datei anpassen
        output_image = "boxplot.png"  # Optional: Bild speichern
        
        # Funktion aufrufen
        create_boxplot_from_csv(csv_file, output_image)
    except Exception as e:
        print(f"Fehler: {e}")
