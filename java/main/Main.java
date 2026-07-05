package main;

import main.controle.ControladorMotorNativo;
import main.controle.ControladorSimulacao;
import main.ui.JanelaPrincipal;

import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;

/**
 * Ponto de entrada da aplicacao.
 *
 * Tudo acontece na Event Dispatch Thread (EDT) do Swing: a inicializacao do
 * motor, o laco de animacao (Timer) e o encerramento. Assim o contrato de
 * thread unica da biblioteca nativa e cumprido sem sincronizacao manual.
 *
 * Note o POLIMORFISMO: a variavel e do tipo da interface
 * {@link ControladorSimulacao}; trocar a implementacao nao afeta a UI.
 */
public final class Main {

    private Main() {
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            final ControladorSimulacao controlador = new ControladorMotorNativo();

            if (!controlador.inicializar()) {
                JOptionPane.showMessageDialog(null,
                        "Falha ao inicializar o motor grafico nativo.",
                        "AlgorithmVision", JOptionPane.ERROR_MESSAGE);
                System.exit(1);
            }

            final JanelaPrincipal janela = new JanelaPrincipal(controlador);
            janela.setVisible(true);
        });
    }
}
