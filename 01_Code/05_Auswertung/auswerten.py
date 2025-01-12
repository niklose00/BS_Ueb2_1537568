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
    plt.ylabel("Latenzzeiten (ns)", fontsize=12)
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    # Optional: Plot speichern
    if output_file:
        # Ordnerpfad extrahieren und sicherstellen, dass er existiert
        output_dir = os.path.dirname(output_file)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
            print(f"Ordner '{output_dir}' wurde erstellt.")
        
        # Bild speichern
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"Boxplot wurde erfolgreich unter '{output_file}' gespeichert.")
    
    # Plot anzeigen
    # plt.show()

# Beispiel-Aufruf
if __name__ == "__main__":
    try:
        input_folder = "02_Outputs/Csv"  # Ordner mit den CSV-Dateien
        output_folder = "02_Outputs/Boxplots"  # Ordner für die Boxplots
        
        # Sicherstellen, dass der Ausgabeordner existiert
        if not os.path.exists(output_folder):
            os.makedirs(output_folder)
            print(f"Ordner '{output_folder}' wurde erstellt.")
        
        # Alle CSV-Dateien im Eingabeordner durchlaufen

        
        for file_name in os.listdir(input_folder):
            if file_name.endswith(".csv"):  # Nur CSV-Dateien verarbeiten
                input_file = os.path.join(input_folder, file_name)
                output_file = os.path.join(output_folder, f"{file_name.split('.')[0]}_boxplot.png")
                
                print(f"Verarbeite Datei: {input_file}")
                try:
                    create_boxplot_from_csv(input_file, output_file)
                except Exception as e:
                    print(f"Fehler beim Verarbeiten der Datei '{file_name}': {e}")
    except Exception as e:
        print(f"Ein schwerwiegender Fehler ist aufgetreten: {e}")
