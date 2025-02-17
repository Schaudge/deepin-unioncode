// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CMAKEPROJECTGENERATOR_H
#define CMAKEPROJECTGENERATOR_H

#include "services/project/projectservice.h"
#include "services/builder/builderservice.h"

#include <QObject>
#include <QDomDocument>

class CmakeProjectGeneratorPrivate;
class CmakeProjectGenerator : public dpfservice::ProjectGenerator
{
    Q_OBJECT
    CmakeProjectGeneratorPrivate *const d;
public:
    explicit CmakeProjectGenerator();
    ~CmakeProjectGenerator() override;
    static QString toolKitName() { return "cmake"; }
    virtual QStringList supportLanguages() override;
    virtual QStringList supportFileNames() override;
    virtual QDialog* configureWidget(const QString &language,
                                     const QString &projectPath) override;
    virtual bool configure(const dpfservice::ProjectInfo &info = {}) override;
    virtual QStandardItem *createRootItem(const dpfservice::ProjectInfo &info) override;
    virtual void removeRootItem(QStandardItem* root) override;
    virtual QMenu* createItemMenu(const QStandardItem *item) override;

private slots:
    void actionTriggered();
    void setRootItemToView(QStandardItem *root);
    void doBuildCmdExecuteEnd(const BuildCommandInfo &info, int status);
    void doCmakeFileNodeChanged(QStandardItem *root, const QPair<QString, QStringList> &files);
    void actionProperties(const dpfservice::ProjectInfo &info, QStandardItem *item);
    void recursionRemoveItem(QStandardItem *item);

private:
    QMutex mutex;
    QStandardItem *rootItem = nullptr;
};

#endif // CMAKEPROJECTGENERATOR_H
