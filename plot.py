import os
import math
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Crear la carpeta de gráficas si no existe
os.makedirs('./plots', exist_ok=True)

# Configuración visual de las gráficas
sns.set_theme(style="whitegrid")
plt.rcParams.update({'font.size': 11})

def generate_latex_table():
    print("Generating LaTeX table...")
    df = pd.read_csv('./results/results.csv')

    # Empezar a construir el string de LaTeX con el formato exacto requerido
    latex_str = r"""\begin{tabular}{lrrrrrrrrrrrrr}
\toprule
 & \multicolumn{4}{c}{\textbf{Random [10k]}} & \textbf{Greedy} & \multicolumn{4}{c}{\textbf{EA [10x]}} & \multicolumn{4}{c}{\textbf{SA [10x]}} \\
\cmidrule(lr){2-5} \cmidrule(lr){7-10} \cmidrule(lr){11-14}
\textbf{Instance} & best & worst & avg & std & best & best & worst & avg & std & best & worst & avg & std \\
\midrule
"""

    instances = df['instance'].unique()

    # Usamos enumerate empezando en 1 para generar "Instance 1", "Instance 2", etc.
    for i, inst in enumerate(instances, start=1):
        row_data = df[df['instance'] == inst]
        
        def get_metrics(alg):
            alg_data = row_data[row_data['algorithm'] == alg]
            if alg_data.empty:
                return " - & - & - & - "
            b = int(alg_data['best'].values[0])
            w = int(alg_data['worst'].values[0])
            a = int(round(alg_data['avg'].values[0]))
            s = int(round(alg_data['std'].values[0]))
            return f"{b} & {w} & {a} & {s}"
            
        def get_greedy_best():
            alg_data = row_data[row_data['algorithm'] == 'Greedy']
            if alg_data.empty:
                return " - "
            return f"{int(alg_data['best'].values[0])}"

        rand_str = get_metrics('Random')
        greedy_str = get_greedy_best()
        ea_str = get_metrics('EA')
        sa_str = get_metrics('SA')
        
        # Formatear el nombre como "Instance X"
        instance_label = f"Instance {i}"
        
        # Añadir la fila al string de LaTeX
        latex_str += f"{instance_label} & {rand_str} & {greedy_str} & {ea_str} & {sa_str} \\\\\n"

    # Cerrar la tabla
    latex_str += r"""\bottomrule
\end{tabular}
"""

    # Guardar el archivo
    with open('./results/latex_table.tex', 'w') as f:
        f.write(latex_str)
    print("-> LaTeX table saved to ./results/latex_table.tex")

def plot_comparisons_subplot():
    print("Generating comparisons subplot...")
    df = pd.read_csv("./results/results.csv")
    instances = df['instance'].unique()
    
    cols = 2
    rows = math.ceil(len(instances) / cols)
    
    fig, axes = plt.subplots(rows, cols, figsize=(14, 5 * rows))
    axes = axes.flatten()
    palette = {"Random": "#E74C3C", "Greedy": "#F1C40F", "EA": "#3498DB", "SA": "#2ECC71"}
    
    for i, instance in enumerate(instances, start=1):
        df_inst = df[df['instance'] == instance]
        
        ax = sns.barplot(x="algorithm", y="best", data=df_inst, palette=palette, edgecolor=".2", ax=axes[i-1])
        
        # Usar "Instance X" para los títulos de los gráficos para mantener la coherencia
        axes[i-1].set_title(f'Performance on Instance {i}', fontsize=14, pad=10)
        axes[i-1].set_ylabel('Best Makespan')
        axes[i-1].set_xlabel('')
        
        for p in ax.patches:
            axes[i-1].annotate(format(p.get_height(), '.0f'), 
                        (p.get_x() + p.get_width() / 2., p.get_height()), 
                        ha='center', va='center', 
                        xytext=(0, 8), 
                        textcoords='offset points',
                        fontweight='bold', fontsize=10)
                        
    for j in range(len(instances), len(axes)):
        fig.delaxes(axes[j])
        
    plt.tight_layout()
    plt.savefig('./plots/all_comparisons.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("-> Comparison plot saved to ./plots/all_comparisons.png")

def plot_convergences_subplot():
    print("Generating convergences subplot...")
    df = pd.read_csv("./results/convergence.csv")
    instances = df['instance'].unique()
    
    cols = 2
    rows = math.ceil(len(instances) / cols)
    
    fig, axes = plt.subplots(rows, cols, figsize=(14, 5 * rows))
    axes = axes.flatten()
    
    for i, instance in enumerate(instances, start=1):
        df_inst = df[df['instance'] == instance]
        if df_inst.empty:
            continue
            
        sns.lineplot(x="generation", y="best_fitness", data=df_inst, 
                     color="#2980B9", linewidth=2, label="EA Best Fitness", ax=axes[i-1])
        
        axes[i-1].set_title(f'EA Convergence Curve (Instance {i})', fontsize=14, pad=10)
        axes[i-1].set_xlabel('Generation')
        axes[i-1].set_ylabel('Best Fitness (Makespan)')
        axes[i-1].legend(loc="upper right")
        
    for j in range(len(instances), len(axes)):
        fig.delaxes(axes[j])
        
    plt.tight_layout()
    plt.savefig('./plots/all_convergences.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("-> Convergence plot saved to ./plots/all_convergences.png")

if __name__ == "__main__":
    print("Iniciando procesamiento de resultados...")
    generate_latex_table()
    plot_comparisons_subplot()
    plot_convergences_subplot()
    print("\n¡Todo listo! Compila tu archivo LaTeX ahora.")