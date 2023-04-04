#pragma once

#include <string>

namespace sd::ClaimTypes
{
    const std::string ClaimTypeNamespace = "http://schemas.microsoft.com/ws/2008/06/identity/claims";

    const std::string AuthenticationInstant = ClaimTypeNamespace + "/authenticationinstant";
    const std::string AuthenticationMethod = ClaimTypeNamespace + "/authenticationmethod";
    const std::string CookiePath = ClaimTypeNamespace + "/cookiepath";
    const std::string DenyOnlyPrimarySid = ClaimTypeNamespace + "/denyonlyprimarysid";
    const std::string DenyOnlyPrimaryGroupSid = ClaimTypeNamespace + "/denyonlyprimarygroupsid";
    const std::string DenyOnlyWindowsDeviceGroup = ClaimTypeNamespace + "/denyonlywindowsdevicegroup";
    const std::string Dsa = ClaimTypeNamespace + "/dsa";
    const std::string Expiration = ClaimTypeNamespace + "/expiration";
    const std::string Expired = ClaimTypeNamespace + "/expired";
    const std::string GroupSid = ClaimTypeNamespace + "/groupsid";
    const std::string IsPersistent = ClaimTypeNamespace + "/ispersistent";
    const std::string PrimaryGroupSid = ClaimTypeNamespace + "/primarygroupsid";
    const std::string PrimarySid = ClaimTypeNamespace + "/primarysid";
    const std::string Role = ClaimTypeNamespace + "/role";
    const std::string SerialNumber = ClaimTypeNamespace + "/serialnumber";
    const std::string UserData = ClaimTypeNamespace + "/userdata";
    const std::string Version = ClaimTypeNamespace + "/version";
    const std::string WindowsAccountName = ClaimTypeNamespace + "/windowsaccountname";
    const std::string WindowsDeviceClaim = ClaimTypeNamespace + "/windowsdeviceclaim";
    const std::string WindowsDeviceGroup = ClaimTypeNamespace + "/windowsdevicegroup";
    const std::string WindowsUserClaim = ClaimTypeNamespace + "/windowsuserclaim";
    const std::string WindowsFqbnVersion = ClaimTypeNamespace + "/windowsfqbnversion";
    const std::string WindowsSubAuthority = ClaimTypeNamespace + "/windowssubauthority";

    const std::string ClaimType2005Namespace = "http://schemas.xmlsoap.org/ws/2005/05/identity/claims";

    const std::string Anonymous = ClaimType2005Namespace + "/anonymous";
    const std::string Authentication = ClaimType2005Namespace + "/authentication";
    const std::string AuthorizationDecision = ClaimType2005Namespace + "/authorizationdecision";
    const std::string Country = ClaimType2005Namespace + "/country";
    const std::string DateOfBirth = ClaimType2005Namespace + "/dateofbirth";
    const std::string Dns = ClaimType2005Namespace + "/dns";
    const std::string DenyOnlySid = ClaimType2005Namespace + "/denyonlysid";
    const std::string Email = ClaimType2005Namespace + "/emailaddress";
    const std::string Gender = ClaimType2005Namespace + "/gender";
    const std::string GivenName = ClaimType2005Namespace + "/givenname";
    const std::string Hash = ClaimType2005Namespace + "/hash";
    const std::string HomePhone = ClaimType2005Namespace + "/homephone";
    const std::string Locality = ClaimType2005Namespace + "/locality";
    const std::string MobilePhone = ClaimType2005Namespace + "/mobilephone";
    const std::string Name = ClaimType2005Namespace + "/name";
    const std::string NameIdentifier = ClaimType2005Namespace + "/nameidentifier";
    const std::string OtherPhone = ClaimType2005Namespace + "/otherphone";
    const std::string PostalCode = ClaimType2005Namespace + "/postalcode";
    const std::string Rsa = ClaimType2005Namespace + "/rsa";
    const std::string Sid = ClaimType2005Namespace + "/sid";
    const std::string Spn = ClaimType2005Namespace + "/spn";
    const std::string StateOrProvince = ClaimType2005Namespace + "/stateorprovince";
    const std::string StreetAddress = ClaimType2005Namespace + "/streetaddress";
    const std::string Surname = ClaimType2005Namespace + "/surname";
    const std::string System = ClaimType2005Namespace + "/system";
    const std::string Thumbprint = ClaimType2005Namespace + "/thumbprint";
    const std::string Upn = ClaimType2005Namespace + "/upn";
    const std::string Uri = ClaimType2005Namespace + "/uri";
    const std::string Webpage = ClaimType2005Namespace + "/webpage";
    const std::string X500DistinguishedName = ClaimType2005Namespace + "/x500distinguishedname";

    const std::string ClaimType2009Namespace = "http://schemas.xmlsoap.org/ws/2009/09/identity/claims";
    const std::string Actor = ClaimType2009Namespace + "/actor";
} // namespace sd::ClaimTypes