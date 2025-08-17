// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: The Monero Project

#include "utils/I2PManager.h"
#include <QFileInfo>
#include <QStringList>

#include <QCoreApplication>

#include "utils/Utils.h"
#include "utils/config.h"

I2PManager::I2PManager(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_checkConnectionTimer(new QTimer(this))
{
    connect(m_checkConnectionTimer, &QTimer::timeout, this, &I2PManager::checkConnection);
    connect(m_process, &QProcess::stateChanged, this, &I2PManager::stateChanged);
}

QPointer<I2PManager> I2PManager::m_instance(nullptr);

I2PManager* I2PManager::instance()
{
    if (m_instance.isNull()) {
        m_instance = new I2PManager();
    }
    return m_instance;
}

void I2PManager::init()
{
    i2pHost = conf()->get(Config::socks5Host).toString();
    i2pPort = conf()->get(Config::socks5Port).toString().toUShort();
}

void I2PManager::start()
{
    m_checkConnectionTimer->start(5000);

    if (m_started || m_process->state() != QProcess::NotRunning) {
        return;
    }

    QString executable = QCoreApplication::applicationDirPath() + "/i2pd";
    if (!QFileInfo::exists(executable)) {
        executable = "i2pd"; // fallback to PATH
    }

    QStringList args;
    args << QString("--socksproxy.address=%1").arg(i2pHost);
    args << QString("--socksproxy.port=%1").arg(i2pPort);

    m_process->start(executable, args);
    m_started = true;
}

void I2PManager::stop()
{
    m_process->kill();
    m_started = false;
}

void I2PManager::stateChanged(QProcess::ProcessState state)
{
    if (state == QProcess::NotRunning) {
        setConnectionState(false);
        if (m_started) {
            QTimer::singleShot(1000, this, &I2PManager::start);
        }
    }
}

void I2PManager::checkConnection()
{
    bool open = Utils::portOpen(i2pHost, i2pPort);
    setConnectionState(open);
}

void I2PManager::setConnectionState(bool connected)
{
    if (i2pConnected == connected)
        return;
    i2pConnected = connected;
    emit connectionStateChanged(connected);
}
