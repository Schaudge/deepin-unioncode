// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mavenconfigpropertywidget.h"

#include "services/option/toolchaindata.h"
#include "common/toolchain/toolchain.h"
#include "common/widget/pagewidget.h"

#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>

static const QString kJrePath = QObject::tr("jre path");
static const QString kJreExecute = QObject::tr("jre execute");
static const QString kLaunchConfigPath = QObject::tr("launch mavenConfig");
static const QString kLaunchPackageFile = QObject::tr("launch package");
static const QString kDapPackageFile = QObject::tr("dap package");
static const int kHeadlineWidth = 120;

class MavenDetailPropertyWidgetPrivate
{
    friend class MavenDetailPropertyWidget;
    QComboBox *jdkVersionComboBox{nullptr};
    QComboBox *mvnVersionComboBox{nullptr};
    QLineEdit *mainClass{nullptr};
    QCheckBox *detailBox{nullptr};
    QLineEdit *jreEdit{nullptr};
    QLineEdit *jreExecuteEdit{nullptr};
    QLineEdit *launchCfgPathEdit{nullptr};
    QLineEdit *lanuchCfgFileEdit{nullptr};
    QLineEdit *dapPackageFileEdit{nullptr};
    QSharedPointer<ToolChainData> toolChainData;
};

MavenDetailPropertyWidget::MavenDetailPropertyWidget(QWidget *parent)
    : QWidget(parent)
    , d(new MavenDetailPropertyWidgetPrivate())
{
    setupUI();
    initData();
}

MavenDetailPropertyWidget::~MavenDetailPropertyWidget()
{
    if (d)
        delete d;
}

void MavenDetailPropertyWidget::setupUI()
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    setLayout(vLayout);

    QHBoxLayout *hLayout = new QHBoxLayout();
    QLabel *label = new QLabel(QLabel::tr("JDK version:"));
    label->setFixedWidth(kHeadlineWidth);
    d->jdkVersionComboBox = new QComboBox();
    hLayout->addWidget(label);
    hLayout->addWidget(d->jdkVersionComboBox);
    vLayout->addLayout(hLayout);

    hLayout = new QHBoxLayout();
    label = new QLabel(QLabel::tr("Maven Version: "));
    label->setFixedWidth(kHeadlineWidth);
    d->mvnVersionComboBox = new QComboBox();
    hLayout->addWidget(label);
    hLayout->addWidget(d->mvnVersionComboBox);
    vLayout->addLayout(hLayout);

    hLayout = new QHBoxLayout();
    label = new QLabel(QLabel::tr("Main Class:"));
    label->setFixedWidth(kHeadlineWidth);
    d->mainClass = new QLineEdit();
    d->mainClass->setPlaceholderText(tr("Input main class"));
    hLayout->addWidget(label);
    hLayout->addWidget(d->mainClass);
    vLayout->addLayout(hLayout);
    vLayout->addStretch(10);

    hLayout = new QHBoxLayout();
    label = new QLabel(QLabel::tr("Detail output:"));
    label->setFixedWidth(kHeadlineWidth);
    d->detailBox = new QCheckBox();
    hLayout->addWidget(label);
    hLayout->addWidget(d->detailBox);
    hLayout->setAlignment(Qt::AlignLeft);
    vLayout->addLayout(hLayout);

    // Dap plugins mavenConfig.
    auto addGroupWidgets = [this](QVBoxLayout *vLayout, const QString &headLine, QWidget *widget){

        QHBoxLayout *hLayout = new QHBoxLayout();
        QLabel *label = new QLabel(headLine + ":");
        label->setFixedWidth(kHeadlineWidth);

        QPushButton *btnBrowser = new QPushButton(this);
        btnBrowser->setText(tr("Browse..."));
        btnBrowser->setObjectName(headLine);
        hLayout->addWidget(label);
        hLayout->addWidget(widget);
        hLayout->addWidget(btnBrowser);
        vLayout->addLayout(hLayout);

        connect(btnBrowser, &QPushButton::clicked, this, &MavenDetailPropertyWidget::browserFileDialog);
    };

    d->jreEdit = new QLineEdit(this);
    d->jreExecuteEdit = new QLineEdit(this);
    d->launchCfgPathEdit = new QLineEdit(this);
    d->lanuchCfgFileEdit = new QLineEdit(this);
    d->dapPackageFileEdit = new QLineEdit(this);

    addGroupWidgets(vLayout, kJrePath, d->jreEdit);
    addGroupWidgets(vLayout, kJreExecute, d->jreExecuteEdit);
    addGroupWidgets(vLayout, kLaunchConfigPath, d->launchCfgPathEdit);
    addGroupWidgets(vLayout, kLaunchPackageFile, d->lanuchCfgFileEdit);
    addGroupWidgets(vLayout, kDapPackageFile,d->dapPackageFileEdit);
}

void MavenDetailPropertyWidget::initData()
{
    d->toolChainData.reset(new ToolChainData());
    QString retMsg;
    bool ret = d->toolChainData->readToolChainData(retMsg);
    if (ret) {
        const ToolChainData::ToolChains &data = d->toolChainData->getToolChanins();
        auto initComboBox = [](QComboBox *comboBox, const ToolChainData::ToolChains &data, const QString &type) {
            int index = 0;
            ToolChainData::Params params = data.value(type);
            for (auto param : params) {
                QString text = param.name + "(" + param.path + ")";
                comboBox->insertItem(index, text);
                comboBox->setItemData(index, QVariant::fromValue(param), Qt::UserRole + 1);
                index++;
            }
        };

        initComboBox(d->jdkVersionComboBox, data, kJDK);
        initComboBox(d->mvnVersionComboBox, data, kMaven);
    }
}

void MavenDetailPropertyWidget::setValues(const mavenConfig::ConfigureParam *param)
{
    if (!param)
        return;

    auto initComboBox = [](QComboBox *comboBox, const mavenConfig::ItemInfo &itemInfo) {
        int count = comboBox->count();
        for (int i = 0; i < count; i++) {
            ToolChainData::ToolChainParam toolChainParam = qvariant_cast<ToolChainData::ToolChainParam>(comboBox->itemData(i, Qt::UserRole + 1));
            if (itemInfo.name == toolChainParam.name
                    && itemInfo.path == toolChainParam.path) {
                comboBox->setCurrentIndex(i);
                break;
            }
        }
    };

    initComboBox(d->jdkVersionComboBox, param->jdkVersion);
    initComboBox(d->mvnVersionComboBox, param->mavenVersion);
    d->mainClass->setText(param->mainClass);
    d->detailBox->setChecked(param->detailInfo);
    d->jreEdit->setText(param->jrePath);
    d->jreExecuteEdit->setText(param->jreExecute);
    d->launchCfgPathEdit->setText(param->launchConfigPath);
    d->lanuchCfgFileEdit->setText(param->launchPackageFile);
    d->dapPackageFileEdit->setText(param->dapPackageFile);
}

void MavenDetailPropertyWidget::getValues(mavenConfig::ConfigureParam *param)
{
    if (!param)
        return;

    auto getValue = [](QComboBox *comboBox, mavenConfig::ItemInfo &itemInfo){
        itemInfo.clear();
        int index = comboBox->currentIndex();
        if (index > -1) {
            ToolChainData::ToolChainParam value = qvariant_cast<ToolChainData::ToolChainParam>(comboBox->itemData(index, Qt::UserRole + 1));
            itemInfo.name = value.name;
            itemInfo.path = value.path;
        }
    };

    getValue(d->jdkVersionComboBox, param->jdkVersion);
    getValue(d->mvnVersionComboBox, param->mavenVersion);
    param->mainClass = d->mainClass->text();
    param->detailInfo = d->detailBox->isChecked();
    param->jrePath = d->jreEdit->text();
    param->jreExecute = d->jreExecuteEdit->text();
    param->launchConfigPath = d->launchCfgPathEdit->text();
    param->launchPackageFile = d->lanuchCfgFileEdit->text();
    param->dapPackageFile = d->dapPackageFileEdit->text();
}

void MavenDetailPropertyWidget::browserFileDialog()
{
    QObject *senderObj = qobject_cast<QObject *>(sender());
    QString senderName = senderObj->objectName();

    auto showDirDialog = [this](QLineEdit *widget){
        QString result = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                           widget->text(),
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!result.isEmpty()) {
            widget->setText(result);
        }
    };

    auto showFileDialog = [this](QLineEdit *widget){
        QString result = QFileDialog::getOpenFileName(this, tr("Select File"), widget->text());
        if (!result.isEmpty())
            widget->setText(result);
    };

    if (senderName == kJrePath) {
        showDirDialog(d->jreEdit);
    } else if (senderName == kJreExecute) {
        showFileDialog(d->jreExecuteEdit);
    } else if (senderName == kLaunchConfigPath) {
        showDirDialog(d->launchCfgPathEdit);
    } else if (senderName == kLaunchPackageFile) {
        showFileDialog(d->lanuchCfgFileEdit);
    } else if (senderName == kDapPackageFile) {
        showFileDialog(d->dapPackageFileEdit);
    }
}

class MavenConfigPropertyWidgetPrivate
{
    friend class MavenConfigPropertyWidget;

    MavenDetailPropertyWidget *detail{nullptr};
    QStandardItem *item{nullptr};
    dpfservice::ProjectInfo projectInfo;
};

MavenConfigPropertyWidget::MavenConfigPropertyWidget(const dpfservice::ProjectInfo &projectInfo, QStandardItem *item, QWidget *parent)
    : PageWidget(parent)
    , d(new MavenConfigPropertyWidgetPrivate())
{
    d->item = item;
    d->projectInfo = projectInfo;

    setupUI();
    initData(projectInfo);
}

MavenConfigPropertyWidget::~MavenConfigPropertyWidget()
{
    if (d)
        delete d;
}

void MavenConfigPropertyWidget::setupUI()
{
    QVBoxLayout *vLayout = new QVBoxLayout();
    setLayout(vLayout);

    d->detail = new MavenDetailPropertyWidget();
    vLayout->addWidget(d->detail);
    vLayout->addStretch(10);
}

void MavenConfigPropertyWidget::initData(const dpfservice::ProjectInfo &projectInfo)
{
    mavenConfig::ConfigureParam *param = mavenConfig::ConfigUtil::instance()->getConfigureParamPointer();
    param->kit = projectInfo.kitName();
    param->language = projectInfo.language();
    param->projectPath = projectInfo.workspaceFolder();
    param->detailInfo = projectInfo.detailInformation();
    param->jrePath = projectInfo.property(mavenConfig::kJrePath).toString();
    param->jreExecute = projectInfo.property(mavenConfig::kJreExecute).toString();
    param->launchConfigPath = projectInfo.property(mavenConfig::kLaunchConfigPath).toString();
    param->launchPackageFile = projectInfo.property(mavenConfig::kLaunchPackageFile).toString();
    param->dapPackageFile = projectInfo.property(mavenConfig::kDapPackageFile).toString();

    d->detail->setValues(param);
}

void MavenConfigPropertyWidget::saveConfig()
{
    mavenConfig::ConfigureParam *param = mavenConfig::ConfigUtil::instance()->getConfigureParamPointer();
    d->detail->getValues(param);

    QString filePath = mavenConfig::ConfigUtil::instance()->getConfigPath(param->projectPath);
    mavenConfig::ConfigUtil::instance()->saveConfig(filePath, *param);

    mavenConfig::ConfigUtil::instance()->updateProjectInfo(d->projectInfo, param);
    dpfservice::ProjectInfo::set(d->item, d->projectInfo);
}
