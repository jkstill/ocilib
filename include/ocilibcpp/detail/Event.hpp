/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "ocilibcpp/types.hpp"

namespace ocilib
{

inline Event::Event(OCI_Event *pEvent)
{
    AcquireTransient(pEvent, Environment::GetEnvironmentHandle());
}

inline Event::EventType Event::GetType() const
{
    return EventType(static_cast<EventType::Type>(core::Check(OCI_EventGetType(*this))));
}

inline Event::ObjectEvent Event::GetObjectEvent() const
{
    return ObjectEvent(static_cast<ObjectEvent::Type>(core::Check(OCI_EventGetOperation(*this))));
}

inline ostring Event::GetDatabaseName() const
{
    return core::MakeString(core::Check(OCI_EventGetDatabase(*this)));
}

inline ostring Event::GetObjectName() const
{
    return core::MakeString(core::Check(OCI_EventGetObject(*this)));
}

inline ostring Event::GetRowID() const
{
    return core::MakeString(core::Check(OCI_EventGetRowid(*this)));
}

inline Subscription Event::GetSubscription() const
{
    return Subscription(core::Check(OCI_EventGetSubscription(*this)));
}

}
