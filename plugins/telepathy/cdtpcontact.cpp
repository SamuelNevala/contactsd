/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (people-users@projects.maemo.org)
**
** This file is part of contactsd.
**
** If you have questions regarding the use of this file, please contact
** Nokia at people-users@projects.maemo.org.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <TelepathyQt4/AvatarData>

#include "cdtpaccount.h"
#include "cdtpcontact.h"
#include "debug.h"

using namespace Contactsd;

CDTpContact::CDTpContact(Tp::ContactPtr contact, CDTpAccount *accountWrapper)
    : QObject(),
      mContact(contact),
      mAccountWrapper(accountWrapper),
      mRemoved(false),
      mQueuedChanges(0)
{
    mQueuedChangesTimer.setInterval(0);
    mQueuedChangesTimer.setSingleShot(true);
    connect(&mQueuedChangesTimer, SIGNAL(timeout()), SLOT(onQueuedChangesTimeout()));

    updateVisibility();

    connect(contact.data(),
            SIGNAL(aliasChanged(const QString &)),
            SLOT(onContactAliasChanged()));
    connect(contact.data(),
            SIGNAL(presenceChanged(const Tp::Presence &)),
            SLOT(onContactPresenceChanged()));
    connect(contact.data(),
            SIGNAL(capabilitiesChanged(const Tp::ContactCapabilities &)),
            SLOT(onContactCapabilitiesChanged()));
    connect(contact.data(),
            SIGNAL(avatarDataChanged(const Tp::AvatarData &)),
            SLOT(onContactAvatarDataChanged()));
    connect(contact.data(),
            SIGNAL(subscriptionStateChanged(Tp::Contact::PresenceState)),
            SLOT(onContactAuthorizationChanged()));
    connect(contact.data(),
            SIGNAL(publishStateChanged(Tp::Contact::PresenceState, const QString &)),
            SLOT(onContactAuthorizationChanged()));
    connect(contact.data(),
            SIGNAL(infoFieldsChanged(const Tp::Contact::InfoFields &)),
            SLOT(onContactInfoChanged()));
    connect(contact.data(),
            SIGNAL(blockStatusChanged(bool)),
            SLOT(onBlockStatusChanged()));
}

CDTpContact::~CDTpContact()
{
}

CDTpAccountPtr CDTpContact::accountWrapper() const
{
    return CDTpAccountPtr(mAccountWrapper);
}

bool CDTpContact::isAvatarKnown() const
{
    if (!mContact->isAvatarTokenKnown()) {
        return false;
    }

    /* If we have a token but not an avatar filename, that probably means the
     * avatar data is being requested and we'll get an update later. */
    if (!mContact->avatarToken().isEmpty() &&
        mContact->avatarData().fileName.isEmpty()) {
        return false;
    }

    return true;
}

bool CDTpContact::isInformationKnown() const
{
    return mContact->isContactInfoKnown();
}

void CDTpContact::onContactAliasChanged()
{
    emitChanged(Alias);
}

void CDTpContact::onContactPresenceChanged()
{
    emitChanged(Presence);
}

void CDTpContact::onContactCapabilitiesChanged()
{
    emitChanged(Capabilities);
}

void CDTpContact::onContactAvatarDataChanged()
{
    emitChanged(Avatar);
}

void CDTpContact::onContactAuthorizationChanged()
{
    emitChanged(Authorization);
}

void CDTpContact::onContactInfoChanged()
{
    emitChanged(Information);
}

void CDTpContact::onBlockStatusChanged()
{
    emitChanged(Blocked);
}

void CDTpContact::emitChanged(CDTpContact::Changes changes)
{
    mQueuedChanges |= changes;
    mQueuedChangesTimer.start();
}

void CDTpContact::onQueuedChangesTimeout()
{
    // Check if this change also modified the visibility
    bool wasVisible = mVisible;
    updateVisibility();
    if (mVisible != wasVisible) {
        mQueuedChanges |= Visibility;
    }

    Q_EMIT changed(CDTpContactPtr(this), mQueuedChanges);

    mQueuedChanges = 0;
}

void CDTpContact::updateVisibility()
{
    /* Don't import contacts blocked, removed or incoming auth requests (because
     * user never asked for them). Note that we still import contacts that have
     * publishState==subscribeState==No, because that case happens if we sent an
     * auth request but it got rejected. In the case we received an auth request
     * and we rejected it, with our implementation, the contact is completely
     * removed from the roster so it won't appear here at all (some other IM
     * clients could still keep the contact in the roster with
     * publishState==subscribeState==No, but that's really corner case so we
     * don't care). */
    mVisible = !mRemoved && !mContact->isBlocked() &&
        (mContact->publishState() != Tp::Contact::PresenceStateAsk ||
         mContact->subscriptionState() != Tp::Contact::PresenceStateNo);
}

void CDTpContact::setRemoved(bool value)
{
    mRemoved = value;
    updateVisibility();
}

