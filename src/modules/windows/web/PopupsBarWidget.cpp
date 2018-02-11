/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2015 Jan Bajer aka bajasoft <jbajer@gmail.com>
* Copyright (C) 2015 - 2018 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "PopupsBarWidget.h"
#include "../../../core/ThemesManager.h"
#include "../../../ui/MainWindow.h"

#include "ui_PopupsBarWidget.h"

#include <QtWidgets/QMenu>

namespace Otter
{

PopupsBarWidget::PopupsBarWidget(const QUrl &parentUrl, bool isPrivate, QWidget *parent) : QWidget(parent),
	m_parentUrl(parentUrl),
	m_isPrivate(isPrivate),
	m_ui(new Ui::PopupsBarWidget)
{
	m_ui->setupUi(this);

	QMenu *menu(new QMenu(this));

	m_ui->iconLabel->setPixmap(ThemesManager::createIcon(QLatin1String("window-popup-block"), false).pixmap(m_ui->iconLabel->size()));
	m_ui->detailsButton->setMenu(menu);
	m_ui->detailsButton->setPopupMode(QToolButton::InstantPopup);

	connect(m_ui->closeButton, &QToolButton::clicked, this, &PopupsBarWidget::requestedClose);
	connect(menu, &QMenu::aboutToShow, this, &PopupsBarWidget::populateMenu);
	connect(menu, &QMenu::aboutToHide, menu, &QMenu::clear);
}

PopupsBarWidget::~PopupsBarWidget()
{
	delete m_ui;
}

void PopupsBarWidget::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);

	if (event->type() == QEvent::LanguageChange)
	{
		m_ui->retranslateUi(this);
	}
}

void PopupsBarWidget::addPopup(const QUrl &url)
{
	m_popupUrls.append(url);

	m_ui->messageLabel->setText(tr("%1 wants to open %n pop-up window(s).", "", m_popupUrls.count()).arg(Utils::extractHost(m_parentUrl)));
}

void PopupsBarWidget::openUrl(QAction *action)
{
	MainWindow *mainWindow(MainWindow::findMainWindow(this));

	if (!action || !mainWindow)
	{
		return;
	}

	const SessionsManager::OpenHints hints(m_isPrivate ? (SessionsManager::NewTabOpen | SessionsManager::PrivateOpen) : SessionsManager::NewTabOpen);

	if (action->data().isNull())
	{
		for (int i = 0; i < m_popupUrls.count(); ++i)
		{
			mainWindow->triggerAction(ActionsManager::OpenUrlAction, {{QLatin1String("url"), m_popupUrls.at(i)}, {QLatin1String("hints"), QVariant(hints)}});
		}
	}
	else
	{
		mainWindow->triggerAction(ActionsManager::OpenUrlAction, {{QLatin1String("url"), action->data().toUrl()}, {QLatin1String("hints"), QVariant(hints)}});
	}
}

void PopupsBarWidget::populateMenu()
{
	const QString popupsPolicy(SettingsManager::getOption(SettingsManager::Permissions_ScriptsCanOpenWindowsOption, Utils::extractHost(m_parentUrl)).toString());
	QMenu *menu(m_ui->detailsButton->menu());
	QAction *openAllAction(menu->addAction(tr("Open All Pop-Ups from This Website")));
	openAllAction->setCheckable(true);
	openAllAction->setChecked(popupsPolicy == QLatin1String("openAll"));
	openAllAction->setData(QLatin1String("openAll"));

	QAction *openAllInBackgroundAction(menu->addAction(tr("Open Pop-Ups from This Website in Background")));
	openAllInBackgroundAction->setCheckable(true);
	openAllInBackgroundAction->setChecked(popupsPolicy == QLatin1String("openAllInBackground"));
	openAllInBackgroundAction->setData(QLatin1String("openAllInBackground"));

	QAction *blockAllAction(menu->addAction(tr("Block All Pop-Ups from This Website")));
	blockAllAction->setCheckable(true);
	blockAllAction->setChecked(popupsPolicy == QLatin1String("blockAll"));
	blockAllAction->setData(QLatin1String("blockAll"));

	QAction *askAction(menu->addAction(tr("Always Ask What to Do for This Website")));
	askAction->setCheckable(true);
	askAction->setChecked(popupsPolicy == QLatin1String("ask"));
	askAction->setData(QLatin1String("ask"));

	QActionGroup *actionGroup(new QActionGroup(this));
	actionGroup->setExclusive(true);
	actionGroup->addAction(openAllAction);
	actionGroup->addAction(openAllInBackgroundAction);
	actionGroup->addAction(blockAllAction);
	actionGroup->addAction(askAction);

	menu->addSeparator();

	QMenu *popupsMenu(menu->addMenu(tr("Blocked Pop-ups")));
	popupsMenu->addAction(tr("Open All"));
	popupsMenu->addSeparator();

	for (int i = 0; i < m_popupUrls.count(); ++i)
	{
		QAction *action(popupsMenu->addAction(Utils::elideText(m_popupUrls.at(i).url(), nullptr, 300)));
		action->setData(m_popupUrls.at(i).url());
	}

	connect(menu, &QMenu::aboutToHide, actionGroup, &QActionGroup::deleteLater);
	connect(popupsMenu, &QMenu::triggered, this, &PopupsBarWidget::openUrl);
	connect(actionGroup, &QActionGroup::triggered, [&](QAction *action)
	{
		if (action)
		{
			SettingsManager::setOption(SettingsManager::Permissions_ScriptsCanOpenWindowsOption, action->data(), Utils::extractHost(m_parentUrl));
		}
	});
}

}
