package main;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextArea;
import javax.swing.Timer;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

/**
 * Painel de controle Swing do AlgorithmVision.
 *
 * Replica o painel ImGui original, mas do lado Java: escolha de algoritmo,
 * distribuicao dos dados, modo de visualizacao, tamanho do array, velocidade
 * e controles de execucao. As metricas sao atualizadas por polling (Timer).
 */
public class ControlUI extends JFrame {

    private static final long serialVersionUID = 1L;

    /** Mapeamento logaritmico do slider de velocidade: 0..100 -> 0.001s..0.5s. */
    private static final float MIN_TICK = 0.001f;
    private static final float MAX_TICK = 0.5f;

    private final MotorGrafico motor;
    private final JButton startPauseButton;
    private final JLabel statusLabel;
    private final JLabel comparisonsLabel;
    private final JLabel writesLabel;
    private final JTextArea descriptionArea;
    private final Timer pollTimer;

    public ControlUI(MotorGrafico motor) {
        this.motor = motor;

        setTitle("AlgorithmVision - Painel de Controle (Java)");
        setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                shutdown();
            }
        });

        JPanel root = new JPanel();
        root.setLayout(new BoxLayout(root, BoxLayout.Y_AXIS));
        root.setBorder(BorderFactory.createEmptyBorder(10, 12, 10, 12));

        // ----- Algoritmo e dados -----
        String[] algorithms = new String[motor.getAlgorithmCount()];
        for (int i = 0; i < algorithms.length; i++) algorithms[i] = motor.getAlgorithmName(i);
        JComboBox<String> algoCombo = new JComboBox<>(algorithms);

        JComboBox<String> distCombo = new JComboBox<>(new String[] {
                "Aleatório", "Reverso", "Quase Ordenado", "Poucos Únicos"
        });
        JComboBox<String> modeCombo = new JComboBox<>(new String[] {
                "Barras", "Dot Plot", "Circular"
        });

        JSlider sizeSlider = new JSlider(10, 1000, 100);
        JLabel sizeValue = new JLabel("100");
        JSlider speedSlider = new JSlider(0, 100, tickToSlider(0.03f));
        JLabel speedValue = new JLabel(formatTick(0.03f));

        // ----- Descricao pedagogica -----
        descriptionArea = new JTextArea(5, 30);
        descriptionArea.setLineWrap(true);
        descriptionArea.setWrapStyleWord(true);
        descriptionArea.setEditable(false);
        descriptionArea.setOpaque(false);
        descriptionArea.setText(motor.getAlgorithmDescription(0));

        // ----- Controles -----
        startPauseButton = new JButton("Iniciar");
        JButton resetButton = new JButton("Resetar");
        statusLabel = new JLabel("Status: Pronto");
        comparisonsLabel = new JLabel("Comparações: 0");
        writesLabel = new JLabel("Acessos: 0");

        // ----- Listeners (cada acao vira um comando enfileirado no motor) -----
        algoCombo.addActionListener(e -> {
            int idx = algoCombo.getSelectedIndex();
            motor.setAlgorithm(idx);
            descriptionArea.setText(motor.getAlgorithmDescription(idx));
        });
        distCombo.addActionListener(e -> motor.setDistribution(distCombo.getSelectedIndex()));
        modeCombo.addActionListener(e -> motor.setRenderMode(modeCombo.getSelectedIndex()));
        sizeSlider.addChangeListener(e -> {
            sizeValue.setText(String.valueOf(sizeSlider.getValue()));
            if (!sizeSlider.getValueIsAdjusting()) motor.setArraySize(sizeSlider.getValue());
        });
        speedSlider.addChangeListener(e -> {
            float tick = sliderToTick(speedSlider.getValue());
            speedValue.setText(formatTick(tick));
            motor.setTickRate(tick);
        });
        startPauseButton.addActionListener(e -> motor.toggleSorting());
        resetButton.addActionListener(e -> motor.reset());

        // ----- Montagem -----
        root.add(labeled("Algoritmo", algoCombo));
        root.add(labeled("Distribuição", distCombo));
        root.add(labeled("Visualização", modeCombo));
        root.add(labeledSlider("Tamanho (N)", sizeSlider, sizeValue));
        root.add(labeledSlider("Velocidade (s/passo)", speedSlider, speedValue));

        JPanel buttons = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 4));
        buttons.add(startPauseButton);
        buttons.add(resetButton);
        root.add(buttons);

        root.add(leftAligned(statusLabel));
        root.add(leftAligned(comparisonsLabel));
        root.add(leftAligned(writesLabel));

        JPanel infoPanel = new JPanel(new BorderLayout());
        infoPanel.setBorder(BorderFactory.createTitledBorder("Informação Pedagógica"));
        infoPanel.add(descriptionArea, BorderLayout.CENTER);
        root.add(infoPanel);

        setContentPane(root);
        pack();
        setMinimumSize(new Dimension(380, getHeight()));
        setLocation(40, 80);

        // ----- Polling das metricas (10x por segundo) -----
        pollTimer = new Timer(100, e -> refreshMetrics());
        pollTimer.start();
    }

    private void refreshMetrics() {
        if (!motor.isRunning()) {
            // Janela OpenGL foi fechada diretamente: encerra a aplicacao inteira.
            shutdown();
            return;
        }
        boolean sorting = motor.isSorting();
        String status = motor.hasFinished() ? "Finalizado" : (sorting ? "Executando" : "Pronto");
        statusLabel.setText("Status: " + status);
        comparisonsLabel.setText("Comparações: " + motor.getComparisons());
        writesLabel.setText("Acessos: " + motor.getWrites());
        startPauseButton.setText(sorting ? "Pausar" : "Iniciar");
    }

    private void shutdown() {
        pollTimer.stop();
        motor.cleanup();
        dispose();
        // Da tempo do loop de render encerrar e liberar os recursos OpenGL.
        new Thread(() -> {
            try {
                for (int i = 0; i < 20 && motor.isRunning(); i++) Thread.sleep(100);
            } catch (InterruptedException ignored) {
                Thread.currentThread().interrupt();
            }
            System.exit(0);
        }).start();
    }

    // ----- Helpers de layout -----

    private static JPanel labeled(String title, JComboBox<String> combo) {
        JPanel panel = new JPanel(new BorderLayout(8, 0));
        panel.add(new JLabel(title), BorderLayout.WEST);
        panel.add(combo, BorderLayout.CENTER);
        panel.setBorder(BorderFactory.createEmptyBorder(2, 0, 2, 0));
        return panel;
    }

    private static JPanel labeledSlider(String title, JSlider slider, JLabel value) {
        JPanel panel = new JPanel(new BorderLayout(8, 0));
        panel.add(new JLabel(title), BorderLayout.WEST);
        panel.add(slider, BorderLayout.CENTER);
        panel.add(value, BorderLayout.EAST);
        panel.setBorder(BorderFactory.createEmptyBorder(2, 0, 2, 0));
        return panel;
    }

    private static JPanel leftAligned(JLabel label) {
        JPanel panel = new JPanel(new FlowLayout(FlowLayout.LEFT, 0, 2));
        panel.add(label);
        return panel;
    }

    // ----- Conversao slider <-> tick rate (escala logaritmica) -----

    private static float sliderToTick(int slider) {
        double ratio = slider / 100.0;
        return (float) (MIN_TICK * Math.pow(MAX_TICK / MIN_TICK, ratio));
    }

    private static int tickToSlider(float tick) {
        double ratio = Math.log(tick / MIN_TICK) / Math.log(MAX_TICK / MIN_TICK);
        return (int) Math.round(ratio * 100.0);
    }

    private static String formatTick(float tick) {
        return String.format("%.3f", tick);
    }
}
