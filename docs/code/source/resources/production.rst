==========
Production
==========

There are two types of production for cities:

Natural Resources
=================

Natural resource production is the production of goods that a city can produce on its own, without any external resources.
These are :ref:`raw-goods`.

The location of a city within a region as well as the region's properties will determine the types and 
quantities of natural resources that a city can produce.

Comodity Production
===================

Comodities are the products of industry. They are produced from natural resources or other comodities.

Given the the resources and direction to produce a comodity, a city will slowly ramp production of that comodity over time. 
The production of a comodity is not instantaneous.

Internally, a the production is ramped using a ``production_aptitude`` multiplier on a per-resource basis. 
This variable is a value between 0 and 1 that represents the expertise that a city has in producing a given resource.
The ``production_aptitude`` of a given resource is derived from the rate of production of that resource as well as the production
rate of similar resources. For instance, a city that produces screws will have a higher production aptitude for not only screws, but also bolts.

This variable factors into the total ``production_efficiency`` of a city for a given resource, which is the actual 
rate at which a city produces a given resource and can depend on additional factors.

Comodity Consumption
====================

The other side of this is consumption. As a city produces resources, they must be either sold or stored. If a city produces more than
it can sell or store, then production is halted. Since ``production_aptitude`` is derived from the rate of production, a city that 
that stops producing resources will see its production aptitude for that and similar resources drop.

Production Similarity Matrix
============================