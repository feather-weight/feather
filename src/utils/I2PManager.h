// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: The Monero Project

#ifndef FEATHER_I2PMANAGER_H
#define FEATHER_I2PMANAGER_H

#include <QObject>
#include <QPointer>
#include <QProcess>
#include <QTimer>

class I2PManager : public QObject
{
    Q_OBJECT

public:
    explicit I2PManager(QObject *parent = nullptr);
    ~I2PManager() override = default;

    void init();
    void start();
    void stop();

    bool isRunning() const { return m_started; }

    static I2PManager* instance();

    bool i2pConnected = false;
    QString i2pHost = "127.0.0.1";
    quint16 i2pPort = 4447;

signals:
    void connectionStateChanged(bool connected);

private slots:
    void stateChanged(QProcess::ProcessState state);
    void checkConnection();

private:
    void setConnectionState(bool connected);

    static QPointer<I2PManager> m_instance;
    QProcess *m_process;
    QTimer *m_checkConnectionTimer;
    bool m_started = false;
};

inline I2PManager* i2pManager()
{
    return I2PManager::instance();
}

#endif // FEATHER_I2PMANAGER_H
