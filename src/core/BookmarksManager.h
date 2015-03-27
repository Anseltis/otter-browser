/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2013 - 2015 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
* Copyright (C) 2014 Piotr Wójcik <chocimier@tlen.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#ifndef OTTER_BOOKMARKSMANAGER_H
#define OTTER_BOOKMARKSMANAGER_H

#include <QtCore/QObject>

namespace Otter
{

class BookmarksItem;
class BookmarksModel;

class BookmarksManager : public QObject
{
	Q_OBJECT

public:
	static void createInstance(QObject *parent = NULL);
	static void updateVisits(const QUrl &url);
	static void deleteBookmark(const QUrl &url);
	static BookmarksManager* getInstance();
	static BookmarksModel* getModel();
	static BookmarksItem* getBookmark(const QString &keyword);
	static BookmarksItem* getBookmark(quint64 identifier);
	static QStringList getKeywords();
	static QList<QUrl> getUrls();
	static bool hasBookmark(const QUrl &url);
	static bool hasKeyword(const QString &keyword);

protected:
	explicit BookmarksManager(QObject *parent = NULL);

	void timerEvent(QTimerEvent *event);

protected slots:
	void scheduleSave();

private:
	int m_saveTimer;

	static BookmarksManager *m_instance;
	static BookmarksModel *m_model;

signals:
	void modelModified();
};

}

#endif
